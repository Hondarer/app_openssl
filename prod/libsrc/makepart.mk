# build_openssl.py が共有ライブラリを配置する場所。
# sync-app-env.sh は、この OUTPUT_DIR から実行時のライブラリ探索パスを導出する。
# see: app/general/docs/vscode-variables.md
OUTPUT_DIR := $(MYAPP_DIR)/prod/lib
