# OpenSSL 自身の Configure と make/nmake で静的ライブラリを作る。
# makefw はこのディレクトリのソースをコンパイルしない。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md
MAKEFW_BUILD := 1
NO_LINK = 1

.PHONY: pre-build
pre-build:
	+python3 "$(MYAPP_DIR)/bin_internal/build_openssl.py" --app-dir "$(MYAPP_DIR)" --config "$(CONFIG)"

.PHONY: pre-clean
pre-clean:
	python3 "$(MYAPP_DIR)/bin_internal/build_openssl.py" --app-dir "$(MYAPP_DIR)" --clean
