#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include "openssl_calls.h"

const char *mock_openssl_test_caller_file(void)
{
    return __FILE__;
}

int mock_openssl_test_probe_secmem_and_error(unsigned long *first_error, unsigned long *after_drain_error)
{
    unsigned long current;

    ERR_clear_error();
    const BIO_METHOD *method = BIO_s_secmem();
    if (method == NULL)
    {
        return -1;
    }
    BIO *bio = BIO_new(method);
    if (bio == NULL)
    {
        return -2;
    }
    if (BIO_set_mem_eof_return(bio, 0) <= 0)
    {
        (void)BIO_free(bio);
        return -3;
    }
    EVP_PKEY *key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    (void)BIO_free(bio);
    if (key != NULL)
    {
        EVP_PKEY_free(key);
        return -4;
    }
    *first_error = ERR_get_error();
    do
    {
        current = ERR_get_error();
    } while (current != 0UL);
    *after_drain_error = ERR_get_error();
    return 0;
}

const BIO_METHOD *mock_openssl_test_call_bio_s_secmem(int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return BIO_s_secmem();
}

long mock_openssl_test_call_bio_set_mem_eof_return(BIO *bio, int eof_value, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return BIO_set_mem_eof_return(bio, eof_value);
}

EVP_PKEY *mock_openssl_test_call_pem_read_bio_private_key(BIO *bio, EVP_PKEY **key, pem_password_cb *callback,
                                                          void *userdata, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return PEM_read_bio_PrivateKey(bio, key, callback, userdata);
}

void mock_openssl_test_call_evp_pkey_free(EVP_PKEY *key, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    EVP_PKEY_free(key);
}

int mock_openssl_test_call_ssl_ctx_use_certificate(SSL_CTX *context, X509 *certificate, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return SSL_CTX_use_certificate(context, certificate);
}

long mock_openssl_test_call_ssl_ctx_add_extra_chain_cert(SSL_CTX *context, X509 *certificate, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return SSL_CTX_add_extra_chain_cert(context, certificate);
}

int mock_openssl_test_call_ssl_ctx_use_private_key(SSL_CTX *context, EVP_PKEY *key, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return SSL_CTX_use_PrivateKey(context, key);
}

int mock_openssl_test_call_x509_store_add_cert(X509_STORE *store, X509 *certificate, int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return X509_STORE_add_cert(store, certificate);
}

unsigned long mock_openssl_test_call_err_get_error(int *caller_line)
{
    *caller_line = __LINE__ + 1;
    return ERR_get_error();
}
