# mock 関数は、提供元に応じて実行時に libcrypto または libssl を読み込んで委譲する。
# 利用側は mock_openssl だけをリンクし、ssl_app と crypto_app を同時にリンクしない。
