# OpenSSL

作業前に [作業規則](AGENTS.md) を確認してください。

このリポジトリ (ディレクトリ) は [c-modernization-kit](https://github.com/Hondarer/c-modernization-kit) の `app/openssl` として管理される、個別アプリです。  
[OpenSSL](https://www.openssl.org/) を makefw からビルドするためのラッパーを提供します。  
c-modernization-kit のワークスペース内 (`framework/makefw` 等と組み合わせた make ビルド環境) で利用することを前提としたアプリであり、本ディレクトリ単体ではビルドできません。

## 提供する機能

OpenSSL 3.5.9 の libcrypto と libssl を、静的ライブラリとして取り込みます。  
openssl コマンドとマニュアルは作りません。

| 環境 | 静的ライブラリ | リンク指定 |
|---|---|---|
| Linux/GCC | `prod/lib/libssl.a`、`prod/lib/libcrypto.a` | `LIBS += ssl crypto` |
| Windows/MSVC | `prod/lib/libssl.lib`、`prod/lib/libcrypto.lib` | `LIBS += ssl crypto` |

Table: 環境別の OpenSSL 静的ライブラリとリンク指定

共有ライブラリは作りません。  
OS にインストールされた OpenSSL の置き換えは行いません。  
TLS 1.3 と DTLS 1.2、アセンブラー実装は有効です。

Configure には、[INSTALL.md](https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md) に記載の次のオプションを渡します。

| オプション | 理由 |
|---|---|
| `no-shared` | 静的ライブラリだけを作る |
| `no-pinshared` | 静的 libcrypto を利用側の共有ライブラリへ入れたとき、その共有ライブラリをプロセス終了まで固定しない |
| `no-autoload-config` | 初期化時に `openssl.cnf` を自動で読まない |

Table: Configure に渡す主なオプションと理由

`no-autoload-config` を指定する理由は、設定ファイルの場所 (OPENSSLDIR) にビルドした場所の絶対パスが埋め込まれるためです。  
自動で読む設定にすると、配布先でそのパスを作れる利用者が、設定を差し込めます。  
証明書の既定の場所も同じパスを指すため、利用側は CA 証明書のファイルを明示して指定してください。

ほかの app から利用する場合は、その app の `appdeps.mk` に `APP_DEPS += openssl` を記載し、対象の `makepart.mk` にリンク指定を追加します。  
リンク順は ssl の次が crypto です。  
公開ヘッダーは system include として提供し、ソースでは `#include <openssl/ssl.h>` と書きます。  
`APP_DEPS` で取り込むと、Linux では `-isystem`、Windows では `/external:I` と `/external:W0` でヘッダーを参照します。  
この取り込み方では、testfw の警告オプション (Linux) と `/W4` (Windows) でも、公開ヘッダーとよく使うマクロの展開から警告は出ません。  
公開ヘッダーを通常の include パスで参照すると、Linux では構造体の詰め物の警告 (`-Wpadded`) が出ます。  
構造体の配置は ABI の一部であり、ヘッダーにパッチを当てずに system include で参照してください。  
Windows では、`<windows.h>` (`<wincrypt.h>`) と OpenSSL のヘッダーは、どちらを先に取り込んでも問題ありません。

Linux で利用側が共有ライブラリの場合は、OpenSSL の関数を公開しないよう、リンク時に次を追加します。  
根拠は GNU ld の [`--exclude-libs`](https://sourceware.org/binutils/docs/ld/Options.html) です。

```text
-Wl,--exclude-libs,libcrypto.a -Wl,--exclude-libs,libssl.a
```

Windows の静的ビルドは `OPENSSL_OPT_WINDLL` を定義せず、`OPENSSL_EXPORT` は `extern` のままです ([`e_os2.h`](https://github.com/openssl/openssl/blob/openssl-3.5.9/include/openssl/e_os2.h))。  
利用側の DLL は、追加の指定がない限り OpenSSL の関数を公開しません。  
静的オブジェクトは `/MT /Zl` でコンパイルされ、既定の CRT ライブラリ名をオブジェクトへ埋め込みません ([`Configurations/10-main.conf`](https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/10-main.conf) の `VC-noCE-common`)。  
実行ファイルや DLL をリンクするとき、利用側の `MSVC_CRT` が CRT を選びます。  
Windows で追加するシステム ライブラリは `ws2_32.lib`、`gdi32.lib`、`advapi32.lib`、`crypt32.lib`、`user32.lib` です。

## 単体テスト用のモック

OpenSSL の関数の失敗を単体テストで注入するため、Google Mock に対応したモックを提供します。  
静的ライブラリの関数はリンク時に差し替えられないため、テスト対象のソースが取り込む OpenSSL のヘッダーを差し替え、呼び出しをマクロで mock 関数へ置き換えます。

| パス | 内容 |
|---|---|
| `test/include/mock_openssl.h` | `Mock_openssl` クラスと mock 関数の宣言、置換マクロ |
| `test/include_override/openssl/evp.h`、`rand.h` | 実ヘッダーを取り込んだ後に置換マクロを定義する override ヘッダー |
| `test/libsrc/mock_openssl/` | mock 関数の実装。`test/lib/libmock_openssl.a` を作る |

Table: モックの構成

makefw は、テストする app と依存先の app の `test/include_override/` を、テスト対象 (`TEST_SRCS`) のコンパイルだけに追加します。  
このため、`APP_DEPS` で openssl を取り込んだ app のテスト対象は、`<openssl/evp.h>` と `<openssl/rand.h>` を取り込むと置き換えの対象になります。  
テスト コードとモック本体は置き換えません。

置き換える関数は次のとおりです。

- `EVP_CIPHER_CTX_new`、`EVP_CIPHER_CTX_ctrl`
- `EVP_EncryptInit_ex`、`EVP_EncryptUpdate`、`EVP_EncryptFinal_ex`
- `EVP_DecryptInit_ex`、`EVP_DecryptUpdate`、`EVP_DecryptFinal_ex`
- `EVP_MD_CTX_new`、`EVP_DigestInit_ex`、`EVP_DigestUpdate`、`EVP_DigestFinal_ex`
- `RAND_bytes`

テストの `makepart.mk` には `LIBS += mock_openssl crypto` を指定します。  
`mock_openssl` を `crypto` より前に置き、mock 関数が呼ぶ実関数を静的な libcrypto から解決します。  
置き換え対象のヘッダーを取り込むテスト対象を持つテストは、モックを生成しない場合も `mock_openssl` をリンクしてください。  
リンクしないと、置き換えた `mock_<関数名>` が未解決になります。

```cpp
#include <mock_openssl.h>

NiceMock<Mock_openssl> mock_openssl;
EXPECT_CALL(mock_openssl, RAND_bytes(_, _, _, _, _)).WillOnce(Return(0));
```

モックを生成しない場合と、個別の動作を設定していない場合は、OpenSSL の実関数へ委譲します。  
mock 関数の先頭 3 引数は、呼び出し元のファイル名、行番号、関数名です。  
同時に生成できる `Mock_openssl` は 1 個です。

override ヘッダーは GCC の `#include_next` で実ヘッダーを取り込むため、モックは Linux だけでビルドします。  
関数を追加する場合は、`mock_openssl.h` の宣言、置換マクロ、`MOCK_METHOD`、`mock_openssl.cc` の `ON_CALL`、関数ごとの `.cc` を同時に更新します。  
override ヘッダーを新設した場合は、既存の依存ファイル (`.d`) に反映されないため、影響するテストの `obj/` を削除してから再ビルドしてください。

## ビルドと更新

```sh
make -C app/openssl
python3 -X warn_default_encoding -W error::EncodingWarning app/openssl/bin_test/test_build_openssl.py
```

`packages/` の公式アーカイブを `gen/` へ展開し、OpenSSL 自身の Configure と make (Windows では nmake) でビルドします。  
公開ヘッダーを `prod/include/openssl/`、静的ライブラリを `prod/lib/`、ライセンス文を `prod/libsrc/openssl/LICENSE.txt` に配置します。  
展開先、オブジェクト、Configure とビルドのログ、配置物は Git 管理対象外です。  
アセンブルは OpenSSL 自身のビルドが行い、makefw にはアセンブラーの規則を追加しません。

OpenSSL 本体にはパッチを当てません。  
ビルドの設定は、`bin_internal/build_openssl.py` が Configure へ渡す引数で行います。  
Windows では、OpenSSL の公式の設定 (`/W3`) でも出る本体由来の警告 (C4267、C4244、C4133、C4334、C4319) を、内容を確認したうえで Configure の引数で抑制します。  
日本語環境で UTF-8 のソースを読むときの警告 (C4819) は、抑制せずに `-source-charset:utf-8` で読み方を正します。  
Linux のビルドでは警告は出ません。

アーカイブ、ビルド スクリプト、Configure の引数が変わらず、配置物がそろっている場合は、ビルドを省略します。  
`CONFIG=Debug` のときだけ Configure に `--debug` を渡し、`RelWithDebInfo` と `Release` は OpenSSL のリリース設定を使います。  
パッケージの取得元、ハッシュ、更新手順は [packages/README.md](packages/README.md) を参照してください。

### ビルドに必要なツール

Configure と、アセンブラー ソースの生成には Perl が必要です。

| 環境 | ツール |
|---|---|
| Linux | GCC、GNU アセンブラー、make、Perl 5.10 以上 (`FindBin`、`IPC::Cmd`、`Time::Piece` を含む) |
| Windows | Visual Studio の C/C++ ビルド ツール (`cl`、`nmake`、`lib`)、NASM 2.13.3 以上、`strawberry-perl` |

Table: 環境別の OpenSSL のビルドに必要なツール

RPM 系の Linux では、`perl` パッケージだけでは Perl のモジュールが足りないため、`perl-core` を導入します ([NOTES-PERL.md](https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-PERL.md))。  
個別に導入する場合は `perl-IPC-Cmd` と `perl-Time-Piece` を入れ、EL9 以降は `perl-FindBin` も入れます。

Windows の Configure には、`osname` が `MSWin32` となる Perl が必要です ([NOTES-WINDOWS.md](https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md))。  
Git 付属の Perl (`msys`) は使えません。  
PowerShell では、次のコマンドで `MSWin32` と表示されることを確認します。  
二重引用符では `$Config` が PowerShell の変数として展開されるため、単一引用符で囲みます。

```powershell
strawberry-perl -MConfig -e 'print $Config{osname}'
```

ビルド スクリプトは PATH から `strawberry-perl` を探して Configure を起動します。  
[devbin-win](https://github.com/Hondarer/devbin-win) の Strawberry Perl を導入すると、`perl.exe` が `strawberry-perl.exe` に改名され、PATH の後段に追加されます。  
`perl` で起動するものは Git 付属の Perl のまま変わりません。  
Configure は起動した Perl の絶対パスを Makefile に記録するため、以降の nmake も同じ Perl を使います。

Windows の対象は x64 の `VC-WIN64A` です。  
nmake は並列ビルドができないため、Windows のビルドは Linux より時間がかかります。  
Linux の make は、親の make にジョブ サーバーがあれば同じジョブ枠を共有し、無い場合は親の `-jN` または CPU 数で並列にビルドします。

## ライセンス

OpenSSL 本体のライセンスは、アーカイブ内の `LICENSE.txt` (Apache License 2.0) が正本です。  
ビルド後は `prod/libsrc/openssl/LICENSE.txt` でも参照できます。  
アーカイブに NOTICE ファイルはありません。  
静的ライブラリをリンクした成果物を配布する場合は、このライセンス文を添付し、本体の著作権表示とライセンス条件を維持してください。  
app 直下の `LICENSE` は、手動作成したラッパー部分に適用します。
