# AGENTS.md

## 対象

このリポジトリは、OpenSSL 3.5.9 の公式配布アーカイブを静的ライブラリとして取り込むラッパーです。

## 参照先

作業に関係する文書の該当節を参照してください。

- [README.md](README.md)
- パッケージの版と更新では [packages/README.md](packages/README.md)

## 注意点

- `packages/` のアーカイブと、`gen/` へ展開した OpenSSL 本体を直接編集しないでください。
- ビルド オプションの変更は `bin_internal/build_openssl.py` で行い、OpenSSL のソースにはパッチを当てないでください。
- 生成された `prod/include/openssl/` と `prod/lib/` のライブラリを直接編集しないでください。
- スクリプトを変更した場合は、`python3 -X warn_default_encoding -W error::EncodingWarning bin_test/test_build_openssl.py` を実行してください。
