# OpenSSL

作業前に [作業規則](AGENTS.md) を確認してください。

このリポジトリ (ディレクトリ) は [c-modernization-kit](https://github.com/Hondarer/c-modernization-kit) の `app/openssl` として管理される、個別アプリです。  
[OpenSSL](https://www.openssl.org/) を makefw からビルドするためのラッパーを提供します。  
c-modernization-kit のワークスペース内 (`framework/makefw` 等と組み合わせた make ビルド環境) で利用することを前提としたアプリであり、本ディレクトリ単体ではビルドできません。

## 提供する機能

OpenSSL 3.5.9 の libcrypto と libssl を、共有ライブラリとして取り込みます。  
openssl コマンドとマニュアルは作りません。

| 環境 | 実行時に使うファイル | リンク時に使うファイル | リンク指定 |
|---|---|---|---|
| Linux/GCC | `prod/lib/libssl_app.so.3`、`prod/lib/libcrypto_app.so.3` | `libssl_app.so`、`libcrypto_app.so` (シンボリック リンク) | `LIBS += ssl_app crypto_app` |
| Windows/MSVC | `prod/lib/libssl-3-x64_app.dll`、`prod/lib/libcrypto-3-x64_app.dll` | `libssl_app.lib`、`libcrypto_app.lib` (import library) | `LIBS += ssl_app crypto_app` |

Table: 環境別の OpenSSL 共有ライブラリとリンク指定

静的ライブラリは配置しません。  
OS にインストールされた OpenSSL の置き換えは行いません。  
TLS 1.3 と DTLS 1.2、アセンブラー実装は有効です。

### ライブラリの名前

第三者管理ライブラリは、OS やほかの製品の同名ライブラリと区別するため、名前に接尾辞 `_app` を付けます。  
OpenSSL では、Configure のターゲット定義の [`shlib_variant`](https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/README.md) に `_app` を指定します。  
この指定は、次の 3 つに反映されます。

- 共有ライブラリのファイル名
- SONAME (Linux)
- シンボル バージョン (Linux)。`OPENSSL_3.0.0` が `OPENSSL_APP_3.0.0` になります。

Linux では、ライブラリ名が違っても関数名はプロセス全体で 1 つの名前空間に入ります。  
シンボル バージョンが変わることで、このライブラリを使う呼び出しと、OS の OpenSSL を使う呼び出しが、同じプロセスの中でも互いに混ざりません。

共有ライブラリの名前のうち、版の番号 (Linux の `.3`、Windows の `-3-x64`) は OpenSSL のビルドが決めます。  
OpenSSL のビルドが作るリンク用のファイル名 (`libcrypto.so`、`libcrypto.lib`) には `_app` が付かないため、ビルド スクリプトが配置のときに名前を変えます。  
Windows の import library は、名前を変えても参照する DLL 名は変わりません。

### Configure のオプション

Configure には、[INSTALL.md](https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md) に記載の次のオプションを渡します。

| オプション | 理由 |
|---|---|
| `shared` | 共有ライブラリを作る |
| `no-autoload-config` | 初期化時に `openssl.cnf` を自動で読まない |
| `no-padlockeng` | VIA の CPU 専用の PadLock エンジンを作らない |

Table: Configure に渡す主なオプションと理由

ターゲットは、OpenSSL のターゲット (`linux-x86_64` など) を継承して `shlib_variant` だけを追加した定義です。  
ビルド スクリプトが定義ファイルを `gen/shlib-variant.conf` に生成し、Configure の `--config` で渡します。

`no-autoload-config` を指定する理由は、設定ファイルの場所 (OPENSSLDIR) にビルドした場所の絶対パスが埋め込まれるためです。  
自動で読む設定にすると、配布先でそのパスを作れる利用者が、設定を差し込めます。  
証明書の既定の場所も同じパスを指すため、利用側は CA 証明書のファイルを明示して指定してください。

PadLock エンジンは、OpenSSL 3.0 で非推奨となった ENGINE API の機能です。  
GCC 14 で最適化すると、このエンジンのソースに誤った警告 (`-Wstringop-overflow`) が出ます。

### 利用方法

ほかの app から利用する場合は、その app の `appdeps.mk` に `APP_DEPS += openssl` を記載し、対象の `makepart.mk` にリンク指定を追加します。  
libcrypto だけを使う場合は `LIBS += crypto_app`、TLS を使う場合は `LIBS += ssl_app crypto_app` です。  
公開ヘッダーは system include として提供し、ソースでは `#include <openssl/ssl.h>` と書きます。  
`APP_DEPS` で取り込むと、Linux では `-isystem`、Windows では `/external:I` と `/external:W0` でヘッダーを参照します。  
この取り込み方では、testfw の警告オプション (Linux) と `/W4` (Windows) でも、公開ヘッダーとよく使うマクロの展開から警告は出ません。  
公開ヘッダーを通常の include パスで参照すると、Linux では構造体の詰め物の警告 (`-Wpadded`) が出ます。  
構造体の配置は ABI の一部であり、ヘッダーにパッチを当てずに system include で参照してください。  
Windows では、`<windows.h>` (`<wincrypt.h>`) と OpenSSL のヘッダーは、どちらを先に取り込んでも問題ありません。

実行時は、共有ライブラリを探索パスから解決します。  
ワークスペースの探索パス (Linux の `LD_LIBRARY_PATH`、Windows の `PATH`) には、`prod/lib` が含まれます。  
利用側の成果物を配布する場合は、実行時に使うファイル (`libcrypto_app.so.3` など) を同梱してください。

OpenSSL のプロバイダー モジュール (`legacy` など) とエンジンは配置しません。  
既定のプロバイダーは libcrypto に含まれます。

Windows の共有ライブラリは、CRT を `/MD` でリンクします ([`Configurations/10-main.conf`](https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/10-main.conf))。  
Windows で OpenSSL をリンクする app は、CRT の設定 (`MSVC_CRT`) をこれに合わせてください。

## 単体テスト用のモック

OpenSSL の関数の失敗を単体テストで注入するため、Google Mock に対応したモックを提供します。  
テスト対象のソースが取り込む OpenSSL のヘッダーを差し替え、呼び出しをマクロで mock 関数へ置き換えます。  
この方式により、mock 関数は呼び出し元のファイル名、行番号、関数名を受け取ります。

| パス | 内容 |
|---|---|
| `test/include/mock_openssl.h` | `Mock_openssl` クラスと mock 関数の宣言、置換マクロ、実ライブラリの名前 |
| `test/include_override/openssl/evp.h`、`rand.h` | 実ヘッダーを取り込んだ後に置換マクロを定義する override ヘッダー |
| `test/libsrc/mock_openssl/` | mock 関数の実装。`test/lib/` に mock ライブラリ `mock_openssl` を作る |

Table: モックの構成

makefw は、テストする app と依存先の app の `test/include_override/` を、テスト対象 (`TEST_SRCS`) のコンパイルだけに追加します。  
このため、`APP_DEPS` で openssl を取り込んだ app のテスト対象は、`<openssl/evp.h>` と `<openssl/rand.h>` を取り込むと置き換えの対象になります。  
テスト コードとモック本体は置き換えません。

### リンクと実委譲

テストの `makepart.mk` には `LIBS += mock_openssl` を指定します。  
`crypto_app` は同時にリンクしません。

```cpp
#include <mock_openssl.h>

NiceMock<Mock_openssl> mock_openssl;
EXPECT_CALL(mock_openssl, RAND_bytes(_, _, _, _, _)).WillOnce(Return(0));
```

モックを生成しない場合と、個別の動作を設定していない場合は、OpenSSL の実関数へ委譲します。  
実関数は、`mock_openssl.h` の `kLibCryptoName` の共有ライブラリ (Linux は `libcrypto_app.so.3`、Windows は `libcrypto-3-x64_app.dll`) を実行時に読み込んで解決します。  
ライブラリまたは関数を解決できない場合は、理由を標準エラーへ出力して終了します。

mock 関数の先頭 3 引数は、呼び出し元のファイル名、行番号、関数名です。  
同時に生成できる `Mock_openssl` は 1 個です。

### 置き換える関数

テスト対象は実ライブラリをリンクしないため、テスト対象が呼ぶ OpenSSL の関数は、失敗を注入しない関数も含めてすべて置き換えます。  
置き換える関数は次のとおりです。

- `EVP_CIPHER_CTX_new`、`EVP_CIPHER_CTX_free`、`EVP_CIPHER_CTX_ctrl`
- `EVP_EncryptInit_ex`、`EVP_EncryptUpdate`、`EVP_EncryptFinal_ex`
- `EVP_DecryptInit_ex`、`EVP_DecryptUpdate`、`EVP_DecryptFinal_ex`
- `EVP_MD_CTX_new`、`EVP_MD_CTX_free`
- `EVP_DigestInit_ex`、`EVP_DigestUpdate`、`EVP_DigestFinal_ex`
- `EVP_aes_256_gcm`、`EVP_sha256`
- `RAND_bytes`

OpenSSL の公開関数は libcrypto と libssl を合わせて約 6,500 個あるため、ワークスペースの app が使う関数だけを対象にします。  
一覧にない関数をテスト対象が呼ぶと、テストのリンクが未解決シンボルで失敗します。  
その場合は、次の箇所へ関数を追加します。

- `mock_openssl.h` の mock 関数の宣言、置換マクロ、`delegate_real_<関数名>` の宣言、`MOCK_METHOD`
- `mock_openssl.cc` の `ON_CALL`
- 関数ごとの `mock_<関数名>.cc`
- この README.md の一覧

対象の関数を宣言するヘッダーが `evp.h` と `rand.h` 以外の場合は、override ヘッダーを追加します。  
override ヘッダーは、置換マクロを定義する前に、mock が対象とする実ヘッダーをすべて取り込みます。  
置換マクロの定義後に実ヘッダーが取り込まれると、関数宣言がマクロで書き換えられるためです。  
実ヘッダーの取り込みには、Linux では `#include_next` を、Windows では include パスからの相対パス (`<../include/openssl/evp.h>`) を使います。  
override ヘッダーを新設した場合は、既存の依存ファイル (`.d`) に反映されないため、影響するテストの `obj/` を削除してから再ビルドしてください。

### 制約

- 関数形式マクロによる置き換えは、関数を関数ポインターとして渡す使い方には効きません。その場合は実関数への参照が残り、リンクが未解決になります。
- テスト コード自身は置き換えの対象外です。テスト コードから OpenSSL の関数を呼ぶ場合は、`mock_<関数名>` を呼びます。

## ビルドと更新

```sh
make -C app/openssl
python3 -X warn_default_encoding -W error::EncodingWarning app/openssl/bin_test/test_build_openssl.py
```

`packages/` の公式アーカイブを `gen/` へ展開し、OpenSSL 自身の Configure と make (Windows では nmake) でビルドします。  
公開ヘッダーを `prod/include/openssl/`、共有ライブラリを `prod/lib/`、ライセンス文を `prod/libsrc/openssl/LICENSE.txt` に配置します。  
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
共有ライブラリを同梱して配布する場合は、このライセンス文を添付し、本体の著作権表示とライセンス条件を維持してください。  
app 直下の `LICENSE` は、手動作成したラッパー部分に適用します。
