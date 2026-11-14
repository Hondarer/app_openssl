#ifndef _MOCK_OPENSSL_H
#define _MOCK_OPENSSL_H

/* OpenSSL の関数の mock。
 *
 * テスト対象 (TEST_SRCS) は test/include_override/openssl/ の差し替えヘッダーを通してこのヘッダーを取り込み、
 * OpenSSL の関数の呼び出しが mock_<関数名>(__FILE__, __LINE__, __func__, ...) へ置き換わる。
 * mock を生成していない場合と、個別の動作を設定していない場合は、関数の提供元に対応する共有ライブラリを
 * 実行時に読み込み、実関数へ委譲する。利用側は mock_openssl だけをリンクし、実ライブラリをリンクしない。
 * このため、テスト対象が呼ぶ OpenSSL の関数は、失敗を注入しない関数も含めてすべてここへ載せる。
 *
 * 差し替えヘッダー経由の場合、OpenSSL の実ヘッダーは差し替えヘッダー側で取り込み済みである。 */
#ifndef _IN_OVERRIDE_HEADER_OPENSSL_H
    #include <openssl/evp.h>
    #include <openssl/rand.h>
    #include <openssl/ssl.h>
    #include <openssl/x509.h>
    #include <openssl/x509_vfy.h>
    #include <openssl/x509v3.h>
    #include <openssl/pem.h>
    #include <openssl/bio.h>
    #include <openssl/err.h>
    #include <openssl/crypto.h>
#endif

typedef int (*mock_openssl_cert_verify_callback)(X509_STORE_CTX *, void *);

#ifdef __cplusplus
extern "C"
{
#endif

    #define MOCK_OPENSSL_AUTH_API(library, result, name, parameters, arguments, types, matchers, real_parameters, real_arguments, kind) \
        extern result mock_##name parameters;
    #include <mock_openssl_auth_api_table.h>
    #undef MOCK_OPENSSL_AUTH_API

    extern EVP_CIPHER_CTX *mock_EVP_CIPHER_CTX_new(const char *, const int, const char *);
    extern void mock_EVP_CIPHER_CTX_free(const char *, const int, const char *, EVP_CIPHER_CTX *);
    extern int mock_EVP_CIPHER_CTX_ctrl(const char *, const int, const char *, EVP_CIPHER_CTX *, int, int, void *);
    extern int mock_EVP_EncryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                       ENGINE *, const unsigned char *, const unsigned char *);
    extern int mock_EVP_EncryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *,
                                      const unsigned char *, int);
    extern int mock_EVP_EncryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                        int *);
    extern int mock_EVP_DecryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                       ENGINE *, const unsigned char *, const unsigned char *);
    extern int mock_EVP_DecryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *,
                                      const unsigned char *, int);
    extern int mock_EVP_DecryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                        int *);
    extern EVP_MD_CTX *mock_EVP_MD_CTX_new(const char *, const int, const char *);
    extern void mock_EVP_MD_CTX_free(const char *, const int, const char *, EVP_MD_CTX *);
    extern int mock_EVP_DigestInit_ex(const char *, const int, const char *, EVP_MD_CTX *, const EVP_MD *, ENGINE *);
    extern int mock_EVP_DigestUpdate(const char *, const int, const char *, EVP_MD_CTX *, const void *, size_t);
    extern int mock_EVP_DigestFinal_ex(const char *, const int, const char *, EVP_MD_CTX *, unsigned char *,
                                       unsigned int *);
    extern const EVP_CIPHER *mock_EVP_aes_256_gcm(const char *, const int, const char *);
    extern const EVP_MD *mock_EVP_sha256(const char *, const int, const char *);
    extern int mock_RAND_bytes(const char *, const int, const char *, unsigned char *, int);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_OPENSSL_H

    #define TLS_method() mock_TLS_method(__FILE__, __LINE__, __func__)
    #define DTLS_method() mock_DTLS_method(__FILE__, __LINE__, __func__)
    #define SSL_CTX_new(method) mock_SSL_CTX_new(__FILE__, __LINE__, __func__, method)
    #define SSL_CTX_free(context) mock_SSL_CTX_free(__FILE__, __LINE__, __func__, context)
    #define SSL_CTX_set_options(context, options) mock_SSL_CTX_set_options(__FILE__, __LINE__, __func__, context, options)
    #define SSL_CTX_ctrl(context, command, number, pointer) mock_SSL_CTX_ctrl(__FILE__, __LINE__, __func__, context, command, number, pointer)
    #define SSL_CTX_set_cipher_list(context, list) mock_SSL_CTX_set_cipher_list(__FILE__, __LINE__, __func__, context, list)
    #define SSL_CTX_set_ciphersuites(context, list) mock_SSL_CTX_set_ciphersuites(__FILE__, __LINE__, __func__, context, list)
    #define SSL_CTX_set_num_tickets(context, count) mock_SSL_CTX_set_num_tickets(__FILE__, __LINE__, __func__, context, count)
    #define SSL_CTX_set_verify(context, mode, callback) mock_SSL_CTX_set_verify(__FILE__, __LINE__, __func__, context, mode, callback)
    #define SSL_CTX_set_cert_verify_callback(context, callback, userdata) mock_SSL_CTX_set_cert_verify_callback(__FILE__, __LINE__, __func__, context, callback, userdata)
    #define SSL_CTX_set_default_passwd_cb(context, callback) mock_SSL_CTX_set_default_passwd_cb(__FILE__, __LINE__, __func__, context, callback)
    #define SSL_CTX_use_certificate_chain_file(context, path) mock_SSL_CTX_use_certificate_chain_file(__FILE__, __LINE__, __func__, context, path)
    #define SSL_CTX_use_PrivateKey_file(context, path, type) mock_SSL_CTX_use_PrivateKey_file(__FILE__, __LINE__, __func__, context, path, type)
    #define SSL_CTX_check_private_key(context) mock_SSL_CTX_check_private_key(__FILE__, __LINE__, __func__, context)
    #define SSL_CTX_load_verify_locations(context, file, directory) mock_SSL_CTX_load_verify_locations(__FILE__, __LINE__, __func__, context, file, directory)
    #define SSL_CTX_get_cert_store(context) mock_SSL_CTX_get_cert_store(__FILE__, __LINE__, __func__, context)
    #define SSL_new(context) mock_SSL_new(__FILE__, __LINE__, __func__, context)
    #define SSL_free(connection) mock_SSL_free(__FILE__, __LINE__, __func__, connection)
    #define SSL_set_bio(connection, input, output) mock_SSL_set_bio(__FILE__, __LINE__, __func__, connection, input, output)
    #define SSL_set_ex_data(connection, index, data) mock_SSL_set_ex_data(__FILE__, __LINE__, __func__, connection, index, data)
    #define SSL_get_ex_data(connection, index) mock_SSL_get_ex_data(__FILE__, __LINE__, __func__, connection, index)
    #define SSL_get_ex_data_X509_STORE_CTX_idx() mock_SSL_get_ex_data_X509_STORE_CTX_idx(__FILE__, __LINE__, __func__)
    #define SSL_set_options(connection, options) mock_SSL_set_options(__FILE__, __LINE__, __func__, connection, options)
    #define SSL_ctrl(connection, command, number, pointer) mock_SSL_ctrl(__FILE__, __LINE__, __func__, connection, command, number, pointer)
    #define SSL_set_connect_state(connection) mock_SSL_set_connect_state(__FILE__, __LINE__, __func__, connection)
    #define SSL_set_accept_state(connection) mock_SSL_set_accept_state(__FILE__, __LINE__, __func__, connection)
    #define SSL_do_handshake(connection) mock_SSL_do_handshake(__FILE__, __LINE__, __func__, connection)
    #define SSL_read_ex(connection, buffer, capacity, length) mock_SSL_read_ex(__FILE__, __LINE__, __func__, connection, buffer, capacity, length)
    #define SSL_get_error(connection, result) mock_SSL_get_error(__FILE__, __LINE__, __func__, connection, result)
    #define SSL_get0_peer_certificate(connection) mock_SSL_get0_peer_certificate(__FILE__, __LINE__, __func__, connection)
    #define SSL_get_event_timeout(connection, timeout, infinite) mock_SSL_get_event_timeout(__FILE__, __LINE__, __func__, connection, timeout, infinite)
    #define SSL_export_keying_material(connection, output, output_length, label, label_length, context, context_length, use_context) mock_SSL_export_keying_material(__FILE__, __LINE__, __func__, connection, output, output_length, label, label_length, context, context_length, use_context)
    #define BIO_new_file(path, mode) mock_BIO_new_file(__FILE__, __LINE__, __func__, path, mode)
    #define BIO_new(method) mock_BIO_new(__FILE__, __LINE__, __func__, method)
    #define BIO_free(bio) mock_BIO_free(__FILE__, __LINE__, __func__, bio)
    #define BIO_s_mem() mock_BIO_s_mem(__FILE__, __LINE__, __func__)
    #define BIO_s_dgram_mem() mock_BIO_s_dgram_mem(__FILE__, __LINE__, __func__)
    #define BIO_ctrl_pending(bio) mock_BIO_ctrl_pending(__FILE__, __LINE__, __func__, bio)
    #define BIO_write(bio, data, length) mock_BIO_write(__FILE__, __LINE__, __func__, bio, data, length)
    #define BIO_read(bio, data, length) mock_BIO_read(__FILE__, __LINE__, __func__, bio, data, length)
    #define PEM_read_bio_X509(bio, certificate, callback, userdata) mock_PEM_read_bio_X509(__FILE__, __LINE__, __func__, bio, certificate, callback, userdata)
    #define PEM_read_bio_X509_CRL(bio, revocations, callback, userdata) mock_PEM_read_bio_X509_CRL(__FILE__, __LINE__, __func__, bio, revocations, callback, userdata)
    #define X509_free(certificate) mock_X509_free(__FILE__, __LINE__, __func__, certificate)
    #define X509_CRL_free(revocations) mock_X509_CRL_free(__FILE__, __LINE__, __func__, revocations)
    #define X509_STORE_add_crl(store, revocations) mock_X509_STORE_add_crl(__FILE__, __LINE__, __func__, store, revocations)
    #define X509_STORE_set_flags(store, flags) mock_X509_STORE_set_flags(__FILE__, __LINE__, __func__, store, flags)
    #define X509_get_ext_d2i(certificate, identifier, critical, index) mock_X509_get_ext_d2i(__FILE__, __LINE__, __func__, certificate, identifier, critical, index)
    #define GENERAL_NAMES_free(names) mock_GENERAL_NAMES_free(__FILE__, __LINE__, __func__, names)
    #define OPENSSL_sk_num(stack) mock_OPENSSL_sk_num(__FILE__, __LINE__, __func__, stack)
    #define OPENSSL_sk_value(stack, index) mock_OPENSSL_sk_value(__FILE__, __LINE__, __func__, stack, index)
    #define ASN1_STRING_length(string) mock_ASN1_STRING_length(__FILE__, __LINE__, __func__, string)
    #define ASN1_STRING_get0_data(string) mock_ASN1_STRING_get0_data(__FILE__, __LINE__, __func__, string)
    #define X509_STORE_CTX_get_ex_data(store, index) mock_X509_STORE_CTX_get_ex_data(__FILE__, __LINE__, __func__, store, index)
    #define X509_STORE_CTX_get0_cert(store) mock_X509_STORE_CTX_get0_cert(__FILE__, __LINE__, __func__, store)
    #define X509_STORE_CTX_set_error(store, error) mock_X509_STORE_CTX_set_error(__FILE__, __LINE__, __func__, store, error)
    #define X509_STORE_CTX_get_error(store) mock_X509_STORE_CTX_get_error(__FILE__, __LINE__, __func__, store)
    #define X509_verify_cert(store) mock_X509_verify_cert(__FILE__, __LINE__, __func__, store)
    #define X509_cmp_current_time(time) mock_X509_cmp_current_time(__FILE__, __LINE__, __func__, time)
    #define X509_get0_notBefore(certificate) mock_X509_get0_notBefore(__FILE__, __LINE__, __func__, certificate)
    #define X509_get0_notAfter(certificate) mock_X509_get0_notAfter(__FILE__, __LINE__, __func__, certificate)
    #define X509_cmp(left, right) mock_X509_cmp(__FILE__, __LINE__, __func__, left, right)
    #define ERR_clear_error() mock_ERR_clear_error(__FILE__, __LINE__, __func__)
    #define ERR_peek_last_error() mock_ERR_peek_last_error(__FILE__, __LINE__, __func__)

    #define EVP_CIPHER_CTX_new()     mock_EVP_CIPHER_CTX_new(__FILE__, __LINE__, __func__)
    #define EVP_CIPHER_CTX_free(ctx) mock_EVP_CIPHER_CTX_free(__FILE__, __LINE__, __func__, ctx)
    #define EVP_CIPHER_CTX_ctrl(ctx, type, arg, ptr) \
        mock_EVP_CIPHER_CTX_ctrl(__FILE__, __LINE__, __func__, ctx, type, arg, ptr)
    #define EVP_EncryptInit_ex(ctx, type, impl, key, iv) \
        mock_EVP_EncryptInit_ex(__FILE__, __LINE__, __func__, ctx, type, impl, key, iv)
    #define EVP_EncryptUpdate(ctx, out, outl, in, inl) \
        mock_EVP_EncryptUpdate(__FILE__, __LINE__, __func__, ctx, out, outl, in, inl)
    #define EVP_EncryptFinal_ex(ctx, out, outl) mock_EVP_EncryptFinal_ex(__FILE__, __LINE__, __func__, ctx, out, outl)
    #define EVP_DecryptInit_ex(ctx, type, impl, key, iv) \
        mock_EVP_DecryptInit_ex(__FILE__, __LINE__, __func__, ctx, type, impl, key, iv)
    #define EVP_DecryptUpdate(ctx, out, outl, in, inl) \
        mock_EVP_DecryptUpdate(__FILE__, __LINE__, __func__, ctx, out, outl, in, inl)
    #define EVP_DecryptFinal_ex(ctx, out, outl) mock_EVP_DecryptFinal_ex(__FILE__, __LINE__, __func__, ctx, out, outl)
    #define EVP_MD_CTX_new()                    mock_EVP_MD_CTX_new(__FILE__, __LINE__, __func__)
    #define EVP_MD_CTX_free(ctx)                mock_EVP_MD_CTX_free(__FILE__, __LINE__, __func__, ctx)
    #define EVP_DigestInit_ex(ctx, type, impl)  mock_EVP_DigestInit_ex(__FILE__, __LINE__, __func__, ctx, type, impl)
    #define EVP_DigestUpdate(ctx, d, cnt)       mock_EVP_DigestUpdate(__FILE__, __LINE__, __func__, ctx, d, cnt)
    #define EVP_DigestFinal_ex(ctx, md, s)      mock_EVP_DigestFinal_ex(__FILE__, __LINE__, __func__, ctx, md, s)
    #define EVP_aes_256_gcm()                   mock_EVP_aes_256_gcm(__FILE__, __LINE__, __func__)
    #define EVP_sha256()                        mock_EVP_sha256(__FILE__, __LINE__, __func__)
    #define RAND_bytes(buf, num)                mock_RAND_bytes(__FILE__, __LINE__, __func__, buf, num)

#else // _IN_OVERRIDE_HEADER_OPENSSL_H

    #include <gmock/gmock.h>

    /* 実委譲で読み込む libcrypto と libssl の共有ライブラリ名。app/openssl の shlib_variant (_app) と版の番号に対応する。 */
    #if defined(_WIN32)
inline constexpr char kLibCryptoName[] = "libcrypto-3-x64_app.dll";
inline constexpr char kLibSslName[] = "libssl-3-x64_app.dll";
    #else
inline constexpr char kLibCryptoName[] = "libcrypto_app.so.3";
inline constexpr char kLibSslName[] = "libssl_app.so.3";
    #endif

#define MOCK_OPENSSL_AUTH_API(library, result, name, parameters, arguments, types, matchers, real_parameters, real_arguments, kind) \
    extern result delegate_real_##name parameters;
#include <mock_openssl_auth_api_table.h>
#undef MOCK_OPENSSL_AUTH_API

extern EVP_CIPHER_CTX *delegate_real_EVP_CIPHER_CTX_new(const char *, const int, const char *);
extern void delegate_real_EVP_CIPHER_CTX_free(const char *, const int, const char *, EVP_CIPHER_CTX *);
extern int delegate_real_EVP_CIPHER_CTX_ctrl(const char *, const int, const char *, EVP_CIPHER_CTX *, int, int, void *);
extern int delegate_real_EVP_EncryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                            ENGINE *, const unsigned char *, const unsigned char *);
extern int delegate_real_EVP_EncryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                           int *, const unsigned char *, int);
extern int delegate_real_EVP_EncryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                             int *);
extern int delegate_real_EVP_DecryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                            ENGINE *, const unsigned char *, const unsigned char *);
extern int delegate_real_EVP_DecryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                           int *, const unsigned char *, int);
extern int delegate_real_EVP_DecryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                             int *);
extern EVP_MD_CTX *delegate_real_EVP_MD_CTX_new(const char *, const int, const char *);
extern void delegate_real_EVP_MD_CTX_free(const char *, const int, const char *, EVP_MD_CTX *);
extern int delegate_real_EVP_DigestInit_ex(const char *, const int, const char *, EVP_MD_CTX *, const EVP_MD *,
                                           ENGINE *);
extern int delegate_real_EVP_DigestUpdate(const char *, const int, const char *, EVP_MD_CTX *, const void *, size_t);
extern int delegate_real_EVP_DigestFinal_ex(const char *, const int, const char *, EVP_MD_CTX *, unsigned char *,
                                            unsigned int *);
extern const EVP_CIPHER *delegate_real_EVP_aes_256_gcm(const char *, const int, const char *);
extern const EVP_MD *delegate_real_EVP_sha256(const char *, const int, const char *);
extern int delegate_real_RAND_bytes(const char *, const int, const char *, unsigned char *, int);

class Mock_openssl
{
  public:
    #define MOCK_OPENSSL_AUTH_API(library, result, name, parameters, arguments, types, matchers, real_parameters, real_arguments, kind) \
        MOCK_METHOD(result, name, types);
    #include <mock_openssl_auth_api_table.h>
    #undef MOCK_OPENSSL_AUTH_API

    MOCK_METHOD(EVP_CIPHER_CTX *, EVP_CIPHER_CTX_new, (const char *, const int, const char *));
    MOCK_METHOD(void, EVP_CIPHER_CTX_free, (const char *, const int, const char *, EVP_CIPHER_CTX *));
    MOCK_METHOD(int, EVP_CIPHER_CTX_ctrl, (const char *, const int, const char *, EVP_CIPHER_CTX *, int, int, void *));
    MOCK_METHOD(int, EVP_EncryptInit_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                 const unsigned char *, const unsigned char *));
    MOCK_METHOD(int, EVP_EncryptUpdate,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *, const unsigned char *,
                 int));
    MOCK_METHOD(int, EVP_EncryptFinal_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *));
    MOCK_METHOD(int, EVP_DecryptInit_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                 const unsigned char *, const unsigned char *));
    MOCK_METHOD(int, EVP_DecryptUpdate,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *, const unsigned char *,
                 int));
    MOCK_METHOD(int, EVP_DecryptFinal_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *));
    MOCK_METHOD(EVP_MD_CTX *, EVP_MD_CTX_new, (const char *, const int, const char *));
    MOCK_METHOD(void, EVP_MD_CTX_free, (const char *, const int, const char *, EVP_MD_CTX *));
    MOCK_METHOD(int, EVP_DigestInit_ex,
                (const char *, const int, const char *, EVP_MD_CTX *, const EVP_MD *, ENGINE *));
    MOCK_METHOD(int, EVP_DigestUpdate, (const char *, const int, const char *, EVP_MD_CTX *, const void *, size_t));
    MOCK_METHOD(int, EVP_DigestFinal_ex,
                (const char *, const int, const char *, EVP_MD_CTX *, unsigned char *, unsigned int *));
    MOCK_METHOD(const EVP_CIPHER *, EVP_aes_256_gcm, (const char *, const int, const char *));
    MOCK_METHOD(const EVP_MD *, EVP_sha256, (const char *, const int, const char *));
    MOCK_METHOD(int, RAND_bytes, (const char *, const int, const char *, unsigned char *, int));

    Mock_openssl();
    virtual ~Mock_openssl();
};

extern Mock_openssl *_mock_openssl;

#endif // _IN_OVERRIDE_HEADER_OPENSSL_H

#endif // _MOCK_OPENSSL_H
