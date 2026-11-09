# AGENTS.md

## 対象

このリポジトリは、OpenSSL の公式配布アーカイブから、libcrypto と libssl の静的ライブラリをワークスペースへ取り込むラッパーであり、OpenSSL を利用するプログラムの単体テスト向けモックも含みます。

## 参照先

作業に関係する文書の該当節を参照してください。

- [README.md](README.md)
- パッケージの展開と更新では [packages/README.md](packages/README.md)

## 注意点

- `packages/` のアーカイブと、`gen/` へ展開した OpenSSL 本体を直接編集しないでください。
- OpenSSL 本体にはパッチを当てず、ビルドの設定は `bin_internal/build_openssl.py` が Configure へ渡す引数で変更してください。
- 生成された `prod/include/openssl/`、`prod/lib/` のライブラリ、`prod/libsrc/openssl/LICENSE.txt` を直接編集しないでください。
- モックの関数を追加・変更した場合は、README.md の「単体テスト用のモック」の一覧を更新し、利用側 app のテストを実行してください。
- ビルド スクリプトを変更した場合は、`python3 -X warn_default_encoding -W error::EncodingWarning bin_test/test_build_openssl.py` を実行してください。
