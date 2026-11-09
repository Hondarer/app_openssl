# packages

[OpenSSL 3.5.9](https://github.com/openssl/openssl/releases/tag/openssl-3.5.9) の公式アーカイブ `openssl-3.5.9.tar.gz` を同梱します。

- 取得元: <https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz>
- バージョン: 3.5.9
- SHA-256: `603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a`
- ライセンス: アーカイブ内の `LICENSE.txt` (Apache License 2.0)

```sh
curl -L -o app/openssl/packages/openssl-3.5.9.tar.gz \
  https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz
sha256sum app/openssl/packages/openssl-3.5.9.tar.gz
```

更新時は、OpenSSL 3.5 系の最新の修正版を公式のハッシュと照合し、古いアーカイブを新しいものへ置き換えます。  
`packages/README.md` のバージョンとハッシュ、`bin_internal/build_openssl.py` の `EXPECTED_SHA256` と `OPENSSL_VERSION` も同じ内容に更新します。  
アーカイブは常に 1 個を配置します。  
複数ある場合は、対応表に載ったファイル名のうちバージョンが最新のものを採用し、未採用のファイルを警告します。  
対応表にないファイル名が混ざっている場合は、ビルドを停止します。  
アーカイブがない場合は、配置方法を表示してビルドを停止します。

更新後は `make -C app/openssl clean` で以前の成果物を削除し、`make -C app/openssl` で静的ライブラリを作り直します。  
Linux では `prod/lib/libcrypto.a` にアセンブラー成果物が含まれることを確認します。  
Windows での確認は、Visual Studio の開発者コマンド プロンプトで同じ make を実行します。
