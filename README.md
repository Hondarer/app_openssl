# OpenSSL 3.5.9 の静的ライブラリ

作業前に [作業規則](AGENTS.md) を確認してください。

このディレクトリは [c-modernization-kit](https://github.com/Hondarer/c-modernization-kit) の `app/openssl` として管理するラッパーです。  
[OpenSSL](https://www.openssl.org/) 3.5.9 の公式アーカイブから、libcrypto と libssl の静的ライブラリと公開ヘッダーを作ります。  
ビルドはワークスペースの makefw から、OpenSSL の Configure と make、または nmake を呼び出します。  
本ディレクトリ単体ではビルドできません。

## 成果物は libcrypto と libssl

共有ライブラリは作りません。  
Configure には `no-shared` と `no-pinshared` を渡します。  
`no-pinshared` は、静的 libcrypto を利用側の共有ライブラリへ入れたときに、その共有ライブラリがプロセス内の他の OpenSSL を終了まで固定しないようにします。  
根拠は OpenSSL の INSTALL.md です。

<https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md>

TLS 1.3 と DTLS 1.2 は有効です。  
アセンブラー実装も有効です。  
アセンブルは OpenSSL 自身のビルドが行い、makefw にはアセンブラーの規則を追加しません。  
openssl コマンドとマニュアルは作りません。

| 環境 | 静的ライブラリ | 公開ヘッダー |
|---|---|---|
| Linux/GCC | `prod/lib/libcrypto.a`、`prod/lib/libssl.a` | `prod/include/openssl/` |
| Windows/MSVC | `prod/lib/libcrypto.lib`、`prod/lib/libssl.lib` | `prod/include/openssl/` |

Table: 環境別の OpenSSL 静的ライブラリと公開ヘッダー

## 利用側は ssl と crypto を静的にリンクする

利用する app の `appdeps.mk` に `APP_DEPS += openssl` を書きます。  
対象の `makepart.mk` に `LIBS += ssl crypto` を書きます。  
リンク順は ssl の次が crypto です。  
公開ヘッダーは system include です。  
ソースでは `#include <openssl/ssl.h>` と書きます。

Linux では、cplat と porter の共有ライブラリから OpenSSL の関数を公開しないため、リンク時に次を追加します。

```text
-Wl,--exclude-libs,libcrypto.a -Wl,--exclude-libs,libssl.a
```

根拠は GNU ld の `--exclude-libs` です。

<https://sourceware.org/binutils/docs/ld/Options.html>

Windows の静的ビルドは `OPENSSL_OPT_WINDLL` を定義しません。  
`OPENSSL_EXPORT` は `extern` のままです。  
利用側の DLL は、追加の指定がない限り OpenSSL の関数を公開しません。  
根拠は `include/openssl/e_os2.h` です。

<https://github.com/openssl/openssl/blob/openssl-3.5.9/include/openssl/e_os2.h>

Windows の静的オブジェクトは `/MT /Zl` でコンパイルされます。  
`/Zl` は既定の CRT ライブラリ名をオブジェクトへ埋め込みません。  
実行ファイルや DLL をリンクするとき、利用側の `MSVC_CRT` が CRT を選びます。  
このラッパーは `MSVC_CRT` を Configure へ渡しません。  
根拠は `Configurations/10-main.conf` の `VC-noCE-common` です。

<https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/10-main.conf>

Windows で追加するシステム ライブラリは `ws2_32.lib`、`gdi32.lib`、`advapi32.lib`、`crypt32.lib`、`user32.lib` です。

## Linux は GCC と Perl、Windows は MSVC と NASM

Linux は GCC、GNU アセンブラー、Perl 5.10 以上を使います。  
RPM 系では `perl` だけでは足りず、`IPC::Cmd` と `Time::Piece` を含む `perl-core` が必要です。  
Windows は Visual Studio の開発者コマンド プロンプト、Perl、NASM 2.13.3 以上を使います。  
Windows の Perl は Strawberry Perl または ActiveState Perl です。  
`perl -MConfig` の osname が `MSWin32` である必要があります。  
Git 付属の Perl は osname が `msys` であり、Configure に使えません。  
NASM は PATH から探します。  
インストール先のパスは固定しません。

根拠は OpenSSL の NOTES-WINDOWS.md、NOTES-PERL.md、INSTALL.md のアセンブラー表です。

<https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md>

<https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-PERL.md>

```sh
make -C app/openssl
```

`CONFIG=Debug` のときだけ Configure に `--debug` を渡します。  
`RelWithDebInfo` と `Release` は OpenSSL のリリース設定です。  
Windows では、リリース設定の C 最適化は `/O2`、デバッグ設定は `/Od` です。  
Windows の対象は x64 の `VC-WIN64A` です。

展開先、オブジェクト、Configure のログは `gen/` に置きます。  
`gen/` は Git の管理対象外です。  
2 回目以降、アーカイブとスクリプトと Configure 引数が同じで成果物が揃っていれば、ビルドを省略します。

スクリプトの単体テストは次のコマンドです。

```sh
python3 -X warn_default_encoding -W error::EncodingWarning app/openssl/bin_test/test_build_openssl.py
```

## ライセンスはラッパーと OpenSSL で分かれる

`packages/openssl-3.5.9.tar.gz` が公式アーカイブです。  
アーカイブ内の `LICENSE.txt` は Apache License 2.0 です。  
このリポジトリ直下の `LICENSE` は、ラッパーの MIT License です。  
取得元、ハッシュ、更新手順は [packages/README.md](packages/README.md) を参照してください。
