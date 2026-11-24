#include <testfw.h>
#include <mock_openssl.h>
#include "openssl_calls.h"

namespace
{

int dummy_password_cb(char *buf, int size, int rwflag, void *userdata)
{
    (void)buf;
    (void)size;
    (void)rwflag;
    (void)userdata;
    return -1;
}

} // namespace

// Mock_openssl を生成しない呼び出しが実 OpenSSL へ委譲されることの確認
TEST(mockOpensslTest, delegates_to_real_without_mock)
{
    // Arrange
    unsigned long first_error = 0UL;
    unsigned long after_drain_error = 1UL; // [状態] - エラー コードの格納先を初期化する。

    // Pre-Assert

    // Act
    int probe_ret = mock_openssl_test_probe_secmem_and_error(
        &first_error,
        &after_drain_error); // [手順] - Mock_openssl を注入せず BIO_s_secmem()、BIO_set_mem_eof_return()、PEM_read_bio_PrivateKey()、ERR_get_error() を実行する。

    // Assert
    EXPECT_EQ(0, probe_ret);          // [確認_正常系] - 実 OpenSSL への委譲が成功すること。
    EXPECT_NE(0UL, first_error);      // [確認_異常系] - 空の BIO に対する PEM_read_bio_PrivateKey() の失敗で ERR_get_error() が非 0 を返すこと。
    EXPECT_EQ(0UL, after_drain_error); // [確認_正常系] - キュー回収後の ERR_get_error() が 0UL を返すこと。
}

// 注入済み Mock_openssl の未設定呼び出しが実 OpenSSL へ委譲されることの確認
TEST(mockOpensslTest, delegates_to_real_with_default_action)
{
    // Arrange
    NiceMock<Mock_openssl> mock_openssl;
    unsigned long first_error = 0UL;
    unsigned long after_drain_error = 1UL; // [状態] - 既定動作の Mock_openssl とエラー コード格納先を用意する。

    // Pre-Assert

    // Act
    int probe_ret = mock_openssl_test_probe_secmem_and_error(
        &first_error,
        &after_drain_error); // [手順] - 既定動作の Mock_openssl を介して一連の OpenSSL 呼び出しを実行する。
    int caller_line = 0;
    mock_openssl_test_call_evp_pkey_free(nullptr, &caller_line);

    // Assert
    EXPECT_EQ(0, probe_ret);          // [確認_正常系] - 未設定の呼び出しが実 OpenSSL へ委譲されること。
    EXPECT_NE(0UL, first_error);      // [確認_異常系] - 実 OpenSSL の ERR_get_error() から非 0 を受け取ること。
    EXPECT_EQ(0UL, after_drain_error); // [確認_正常系] - 実 OpenSSL のエラー キュー回収後に 0UL を受け取ること。
}

// 追加した 8 関数とマクロ委譲が呼び出し元情報を伴って期待値に置換できることの確認
TEST(mockOpensslTest, overrides_new_auth_functions_and_macros)
{
    // Arrange
    NiceMock<Mock_openssl> mock_openssl;
    const char *expected_file = mock_openssl_test_caller_file();
    BIO *dummy_bio = reinterpret_cast<BIO *>(0x1001);
    SSL_CTX *dummy_ctx = reinterpret_cast<SSL_CTX *>(0x1002);
    X509 *dummy_cert = reinterpret_cast<X509 *>(0x1003);
    EVP_PKEY *dummy_key = reinterpret_cast<EVP_PKEY *>(0x1004);
    X509_STORE *dummy_store = reinterpret_cast<X509_STORE *>(0x1005);
    const BIO_METHOD *dummy_method = reinterpret_cast<const BIO_METHOD *>(0x1006);
    int dummy_userdata = 42;
    EVP_PKEY *out_key = nullptr;
    int line_bio_s_secmem = 0;
    int line_bio_ctrl = 0;
    int line_pem_key = 0;
    int line_evp_free = 0;
    int line_use_cert = 0;
    int line_extra_chain = 0;
    int line_use_key = 0;
    int line_add_cert = 0;
    int line_err_get = 0;
    // [状態] - 置換対象の引数と呼び出し元情報を受け取る変数を初期化する。

    // Pre-Assert
    EXPECT_CALL(mock_openssl, BIO_s_secmem(StrEq(expected_file), _, StrEq("mock_openssl_test_call_bio_s_secmem")))
        .WillOnce(Return(dummy_method));
    EXPECT_CALL(mock_openssl, BIO_ctrl(StrEq(expected_file), _, StrEq("mock_openssl_test_call_bio_set_mem_eof_return"),
                                       dummy_bio, BIO_C_SET_BUF_MEM_EOF_RETURN, 0L, nullptr))
        .WillOnce(Return(1L));
    EXPECT_CALL(mock_openssl,
                PEM_read_bio_PrivateKey(StrEq(expected_file), _, StrEq("mock_openssl_test_call_pem_read_bio_private_key"),
                                        dummy_bio, &out_key, &dummy_password_cb, &dummy_userdata))
        .WillOnce(Return(dummy_key));
    EXPECT_CALL(mock_openssl, EVP_PKEY_free(StrEq(expected_file), _, StrEq("mock_openssl_test_call_evp_pkey_free"), dummy_key))
        .WillOnce(Return());
    EXPECT_CALL(mock_openssl, SSL_CTX_use_certificate(StrEq(expected_file), _,
                                                      StrEq("mock_openssl_test_call_ssl_ctx_use_certificate"),
                                                      dummy_ctx, dummy_cert))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_openssl, SSL_CTX_ctrl(StrEq(expected_file), _,
                                           StrEq("mock_openssl_test_call_ssl_ctx_add_extra_chain_cert"), dummy_ctx,
                                           SSL_CTRL_EXTRA_CHAIN_CERT, 0L, dummy_cert))
        .WillOnce(Return(1L));
    EXPECT_CALL(mock_openssl, SSL_CTX_use_PrivateKey(StrEq(expected_file), _,
                                                     StrEq("mock_openssl_test_call_ssl_ctx_use_private_key"), dummy_ctx,
                                                     dummy_key))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_openssl, X509_STORE_add_cert(StrEq(expected_file), _,
                                                  StrEq("mock_openssl_test_call_x509_store_add_cert"), dummy_store,
                                                  dummy_cert))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_openssl, ERR_get_error(StrEq(expected_file), _, StrEq("mock_openssl_test_call_err_get_error")))
        .WillOnce(Return(0xdeadbeefUL));

    // Act
    const BIO_METHOD *method_ret = mock_openssl_test_call_bio_s_secmem(&line_bio_s_secmem);
    long eof_ret = mock_openssl_test_call_bio_set_mem_eof_return(dummy_bio, 0, &line_bio_ctrl);
    EVP_PKEY *key_ret =
        mock_openssl_test_call_pem_read_bio_private_key(dummy_bio, &out_key, &dummy_password_cb, &dummy_userdata, &line_pem_key);
    mock_openssl_test_call_evp_pkey_free(dummy_key, &line_evp_free);
    int use_cert_ret = mock_openssl_test_call_ssl_ctx_use_certificate(dummy_ctx, dummy_cert, &line_use_cert);
    long extra_chain_ret = mock_openssl_test_call_ssl_ctx_add_extra_chain_cert(dummy_ctx, dummy_cert, &line_extra_chain);
    int use_key_ret = mock_openssl_test_call_ssl_ctx_use_private_key(dummy_ctx, dummy_key, &line_use_key);
    int add_cert_ret = mock_openssl_test_call_x509_store_add_cert(dummy_store, dummy_cert, &line_add_cert);
    unsigned long err_ret = mock_openssl_test_call_err_get_error(&line_err_get);

    // Assert
    EXPECT_EQ(dummy_method, method_ret);
    EXPECT_EQ(1L, eof_ret);
    EXPECT_EQ(dummy_key, key_ret);
    EXPECT_EQ(1, use_cert_ret);
    EXPECT_EQ(1L, extra_chain_ret);
    EXPECT_EQ(1, use_key_ret);
    EXPECT_EQ(1, add_cert_ret);
    EXPECT_EQ(0xdeadbeefUL, err_ret);
    EXPECT_GT(line_bio_s_secmem, 0);
    EXPECT_GT(line_bio_ctrl, line_bio_s_secmem);
    EXPECT_GT(line_pem_key, line_bio_ctrl);
    EXPECT_GT(line_evp_free, line_pem_key);
    EXPECT_GT(line_use_cert, line_evp_free);
    EXPECT_GT(line_extra_chain, line_use_cert);
    EXPECT_GT(line_use_key, line_extra_chain);
    EXPECT_GT(line_add_cert, line_use_key);
    EXPECT_GT(line_err_get, line_add_cert);
}
