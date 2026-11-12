# mock 関数は、実行時に読み込んだ libcrypto の共有ライブラリへ委譲する。
# 利用側は mock_openssl だけをリンクし、crypto_app を同時にリンクしない。
