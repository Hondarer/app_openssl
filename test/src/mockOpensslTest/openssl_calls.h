#ifndef MOCK_OPENSSL_TEST_OPENSSL_CALLS_H
#define MOCK_OPENSSL_TEST_OPENSSL_CALLS_H

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#ifdef __cplusplus
extern "C"
{
#endif

const char *mock_openssl_test_caller_file(void);
int mock_openssl_test_probe_secmem_and_error(unsigned long *first_error, unsigned long *after_drain_error);
const BIO_METHOD *mock_openssl_test_call_bio_s_secmem(int *caller_line);
long mock_openssl_test_call_bio_set_mem_eof_return(BIO *bio, int eof_value, int *caller_line);
EVP_PKEY *mock_openssl_test_call_pem_read_bio_private_key(BIO *bio, EVP_PKEY **key, pem_password_cb *callback,
                                                          void *userdata, int *caller_line);
void mock_openssl_test_call_evp_pkey_free(EVP_PKEY *key, int *caller_line);
int mock_openssl_test_call_ssl_ctx_use_certificate(SSL_CTX *context, X509 *certificate, int *caller_line);
long mock_openssl_test_call_ssl_ctx_add_extra_chain_cert(SSL_CTX *context, X509 *certificate, int *caller_line);
int mock_openssl_test_call_ssl_ctx_use_private_key(SSL_CTX *context, EVP_PKEY *key, int *caller_line);
int mock_openssl_test_call_x509_store_add_cert(X509_STORE *store, X509 *certificate, int *caller_line);
unsigned long mock_openssl_test_call_err_get_error(int *caller_line);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_OPENSSL_TEST_OPENSSL_CALLS_H */
