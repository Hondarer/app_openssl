# packages

[OpenSSL 3.5.9 のリリース](https://github.com/openssl/openssl/releases/tag/openssl-3.5.9) から取得した `openssl-3.5.9.tar.gz` を同梱します。

- 取得元: <https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz>
- バージョン: 3.5.9
- SHA-256: `603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a`
- ライセンス: アーカイブ内の `LICENSE.txt` (Apache License 2.0)

```sh
curl -L -o app/openssl/packages/openssl-3.5.9.tar.gz \
  https://github.com/openssl/openssl/releases/download/openssl-3.5.9/openssl-3.5.9.tar.gz
sha256sum app/openssl/packages/openssl-3.5.9.tar.gz
```

アーカイブは約 50.8[MiB] です。  
GitHub が推奨する 50[MiB] を超えるため、このファイルを含むコミットの push 時に警告が表示されます。  
拒否される上限 (100[MiB]) より小さいため、push はできます。  
ビルド時に公式アーカイブの SHA-256 と照合するため、展開して不要なファイルを除いた縮小版には置き換えません。  
上限の根拠は [GitHub のドキュメント](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-large-files-on-github) です。

更新時は、OpenSSL 3.5 系の最新の修正版を公式のハッシュと照合し、古いアーカイブを新しいものに置き換えます。  
この文書のバージョンとハッシュ、`bin_internal/build_openssl.py` の `EXPECTED_SHA256` と `OPENSSL_VERSION` も同じ内容に更新します。  
アーカイブは常に 1 個を配置します。  
複数ある場合は、対応表に載ったファイル名のうちバージョンが最新のものを選び、未採用のファイルを警告します。  
対応表にないファイル名が混在する場合は、ビルドを停止します。  
アーカイブがない場合は、配置方法を表示してビルドを停止します。

更新後は `make -C app/openssl clean` で以前の成果物を削除し、`make -C app/openssl` で静的ライブラリを作り直します。  
ビルド スクリプトは、配置した `libcrypto` にアセンブラー成果物が含まれることと、`configuration.h` の機能の有効・無効を確認します。  
Windows でも同じ make を、Visual Studio の C/C++ ビルド ツールを使える環境で実行します。
