/* 認証で使用する API。宣言、実委譲、ラッパー、MOCK_METHOD、ON_CALL の共通一覧です。 */
#ifndef MOCK_OPENSSL_AUTH_API
#error "MOCK_OPENSSL_AUTH_API must be defined"
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_TLS_method)
MOCK_OPENSSL_AUTH_API(Ssl, const SSL_METHOD *, TLS_method, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_DTLS_method)
MOCK_OPENSSL_AUTH_API(Ssl, const SSL_METHOD *, DTLS_method, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_new)
MOCK_OPENSSL_AUTH_API(Ssl, SSL_CTX *, SSL_CTX_new, (const char *file, const int line, const char *func, const SSL_METHOD * method), (file, line, func, method), (const char *, const int, const char *, const SSL_METHOD *), (_, _, _, _), (const SSL_METHOD * method), (method), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_free)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_CTX_free, (const char *file, const int line, const char *func, SSL_CTX * context), (file, line, func, context), (const char *, const int, const char *, SSL_CTX *), (_, _, _, _), (SSL_CTX * context), (context), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_options)
MOCK_OPENSSL_AUTH_API(Ssl, uint64_t, SSL_CTX_set_options, (const char *file, const int line, const char *func, SSL_CTX * context, uint64_t options), (file, line, func, context, options), (const char *, const int, const char *, SSL_CTX *, uint64_t), (_, _, _, _, _), (SSL_CTX * context, uint64_t options), (context, options), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_ctrl)
MOCK_OPENSSL_AUTH_API(Ssl, long, SSL_CTX_ctrl, (const char *file, const int line, const char *func, SSL_CTX * context, int command, long number, void * pointer), (file, line, func, context, command, number, pointer), (const char *, const int, const char *, SSL_CTX *, int, long, void *), (_, _, _, _, _, _, _), (SSL_CTX * context, int command, long number, void * pointer), (context, command, number, pointer), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_cipher_list)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_set_cipher_list, (const char *file, const int line, const char *func, SSL_CTX * context, const char * list), (file, line, func, context, list), (const char *, const int, const char *, SSL_CTX *, const char *), (_, _, _, _, _), (SSL_CTX * context, const char * list), (context, list), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_ciphersuites)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_set_ciphersuites, (const char *file, const int line, const char *func, SSL_CTX * context, const char * list), (file, line, func, context, list), (const char *, const int, const char *, SSL_CTX *, const char *), (_, _, _, _, _), (SSL_CTX * context, const char * list), (context, list), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_num_tickets)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_set_num_tickets, (const char *file, const int line, const char *func, SSL_CTX * context, size_t count), (file, line, func, context, count), (const char *, const int, const char *, SSL_CTX *, size_t), (_, _, _, _, _), (SSL_CTX * context, size_t count), (context, count), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_verify)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_CTX_set_verify, (const char *file, const int line, const char *func, SSL_CTX * context, int mode, SSL_verify_cb callback), (file, line, func, context, mode, callback), (const char *, const int, const char *, SSL_CTX *, int, SSL_verify_cb), (_, _, _, _, _, _), (SSL_CTX * context, int mode, SSL_verify_cb callback), (context, mode, callback), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_cert_verify_callback)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_CTX_set_cert_verify_callback, (const char *file, const int line, const char *func, SSL_CTX * context, mock_openssl_cert_verify_callback callback, void * userdata), (file, line, func, context, callback, userdata), (const char *, const int, const char *, SSL_CTX *, mock_openssl_cert_verify_callback, void *), (_, _, _, _, _, _), (SSL_CTX * context, mock_openssl_cert_verify_callback callback, void * userdata), (context, callback, userdata), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_default_passwd_cb)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_CTX_set_default_passwd_cb, (const char *file, const int line, const char *func, SSL_CTX * context, pem_password_cb * callback), (file, line, func, context, callback), (const char *, const int, const char *, SSL_CTX *, pem_password_cb *), (_, _, _, _, _), (SSL_CTX * context, pem_password_cb * callback), (context, callback), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_use_certificate_chain_file)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_use_certificate_chain_file, (const char *file, const int line, const char *func, SSL_CTX * context, const char * path), (file, line, func, context, path), (const char *, const int, const char *, SSL_CTX *, const char *), (_, _, _, _, _), (SSL_CTX * context, const char * path), (context, path), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_use_PrivateKey_file)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_use_PrivateKey_file, (const char *file, const int line, const char *func, SSL_CTX * context, const char * path, int type), (file, line, func, context, path, type), (const char *, const int, const char *, SSL_CTX *, const char *, int), (_, _, _, _, _, _), (SSL_CTX * context, const char * path, int type), (context, path, type), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_check_private_key)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_check_private_key, (const char *file, const int line, const char *func, const SSL_CTX * context), (file, line, func, context), (const char *, const int, const char *, const SSL_CTX *), (_, _, _, _), (const SSL_CTX * context), (context), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_load_verify_locations)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_CTX_load_verify_locations, (const char *file, const int line, const char *func, SSL_CTX * context, const char * authority_file, const char * directory), (file, line, func, context, authority_file, directory), (const char *, const int, const char *, SSL_CTX *, const char *, const char *), (_, _, _, _, _, _), (SSL_CTX * context, const char * authority_file, const char * directory), (context, authority_file, directory), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_get_cert_store)
MOCK_OPENSSL_AUTH_API(Ssl, X509_STORE *, SSL_CTX_get_cert_store, (const char *file, const int line, const char *func, const SSL_CTX * context), (file, line, func, context), (const char *, const int, const char *, const SSL_CTX *), (_, _, _, _), (const SSL_CTX * context), (context), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_new)
MOCK_OPENSSL_AUTH_API(Ssl, SSL *, SSL_new, (const char *file, const int line, const char *func, SSL_CTX * context), (file, line, func, context), (const char *, const int, const char *, SSL_CTX *), (_, _, _, _), (SSL_CTX * context), (context), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_free)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_free, (const char *file, const int line, const char *func, SSL * connection), (file, line, func, connection), (const char *, const int, const char *, SSL *), (_, _, _, _), (SSL * connection), (connection), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_set_bio)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_set_bio, (const char *file, const int line, const char *func, SSL * connection, BIO * input, BIO * output), (file, line, func, connection, input, output), (const char *, const int, const char *, SSL *, BIO *, BIO *), (_, _, _, _, _, _), (SSL * connection, BIO * input, BIO * output), (connection, input, output), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_set_ex_data)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_set_ex_data, (const char *file, const int line, const char *func, SSL * connection, int index, void * data), (file, line, func, connection, index, data), (const char *, const int, const char *, SSL *, int, void *), (_, _, _, _, _, _), (SSL * connection, int index, void * data), (connection, index, data), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_get_ex_data)
MOCK_OPENSSL_AUTH_API(Ssl, void *, SSL_get_ex_data, (const char *file, const int line, const char *func, const SSL * connection, int index), (file, line, func, connection, index), (const char *, const int, const char *, const SSL *, int), (_, _, _, _, _), (const SSL * connection, int index), (connection, index), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_get_ex_data_X509_STORE_CTX_idx)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_get_ex_data_X509_STORE_CTX_idx, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_set_options)
MOCK_OPENSSL_AUTH_API(Ssl, uint64_t, SSL_set_options, (const char *file, const int line, const char *func, SSL * connection, uint64_t options), (file, line, func, connection, options), (const char *, const int, const char *, SSL *, uint64_t), (_, _, _, _, _), (SSL * connection, uint64_t options), (connection, options), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_ctrl)
MOCK_OPENSSL_AUTH_API(Ssl, long, SSL_ctrl, (const char *file, const int line, const char *func, SSL * connection, int command, long number, void * pointer), (file, line, func, connection, command, number, pointer), (const char *, const int, const char *, SSL *, int, long, void *), (_, _, _, _, _, _, _), (SSL * connection, int command, long number, void * pointer), (connection, command, number, pointer), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_set_connect_state)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_set_connect_state, (const char *file, const int line, const char *func, SSL * connection), (file, line, func, connection), (const char *, const int, const char *, SSL *), (_, _, _, _), (SSL * connection), (connection), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_set_accept_state)
MOCK_OPENSSL_AUTH_API(Ssl, void, SSL_set_accept_state, (const char *file, const int line, const char *func, SSL * connection), (file, line, func, connection), (const char *, const int, const char *, SSL *), (_, _, _, _), (SSL * connection), (connection), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_do_handshake)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_do_handshake, (const char *file, const int line, const char *func, SSL * connection), (file, line, func, connection), (const char *, const int, const char *, SSL *), (_, _, _, _), (SSL * connection), (connection), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_read_ex)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_read_ex, (const char *file, const int line, const char *func, SSL * connection, void * buffer, size_t capacity, size_t * length), (file, line, func, connection, buffer, capacity, length), (const char *, const int, const char *, SSL *, void *, size_t, size_t *), (_, _, _, _, _, _, _), (SSL * connection, void * buffer, size_t capacity, size_t * length), (connection, buffer, capacity, length), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_get_error)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_get_error, (const char *file, const int line, const char *func, const SSL * connection, int result), (file, line, func, connection, result), (const char *, const int, const char *, const SSL *, int), (_, _, _, _, _), (const SSL * connection, int result), (connection, result), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_get0_peer_certificate)
MOCK_OPENSSL_AUTH_API(Ssl, X509 *, SSL_get0_peer_certificate, (const char *file, const int line, const char *func, const SSL * connection), (file, line, func, connection), (const char *, const int, const char *, const SSL *), (_, _, _, _), (const SSL * connection), (connection), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_get_event_timeout)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_get_event_timeout, (const char *file, const int line, const char *func, SSL * connection, struct timeval * timeout, int * infinite), (file, line, func, connection, timeout, infinite), (const char *, const int, const char *, SSL *, struct timeval *, int *), (_, _, _, _, _, _), (SSL * connection, struct timeval * timeout, int * infinite), (connection, timeout, infinite), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_SSL_export_keying_material)
MOCK_OPENSSL_AUTH_API(Ssl, int, SSL_export_keying_material, (const char *file, const int line, const char *func, SSL * connection, unsigned char * output, size_t output_length, const char * label, size_t label_length, const unsigned char * context, size_t context_length, int use_context), (file, line, func, connection, output, output_length, label, label_length, context, context_length, use_context), (const char *, const int, const char *, SSL *, unsigned char *, size_t, const char *, size_t, const unsigned char *, size_t, int), (_, _, _, _, _, _, _, _, _, _, _), (SSL * connection, unsigned char * output, size_t output_length, const char * label, size_t label_length, const unsigned char * context, size_t context_length, int use_context), (connection, output, output_length, label, label_length, context, context_length, use_context), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_new_file)
MOCK_OPENSSL_AUTH_API(Crypto, BIO *, BIO_new_file, (const char *file, const int line, const char *func, const char * path, const char * mode), (file, line, func, path, mode), (const char *, const int, const char *, const char *, const char *), (_, _, _, _, _), (const char * path, const char * mode), (path, mode), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_new)
MOCK_OPENSSL_AUTH_API(Crypto, BIO *, BIO_new, (const char *file, const int line, const char *func, const BIO_METHOD * method), (file, line, func, method), (const char *, const int, const char *, const BIO_METHOD *), (_, _, _, _), (const BIO_METHOD * method), (method), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_free)
MOCK_OPENSSL_AUTH_API(Crypto, int, BIO_free, (const char *file, const int line, const char *func, BIO * bio), (file, line, func, bio), (const char *, const int, const char *, BIO *), (_, _, _, _), (BIO * bio), (bio), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_s_mem)
MOCK_OPENSSL_AUTH_API(Crypto, const BIO_METHOD *, BIO_s_mem, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_s_dgram_mem)
MOCK_OPENSSL_AUTH_API(Crypto, const BIO_METHOD *, BIO_s_dgram_mem, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_ctrl_pending)
MOCK_OPENSSL_AUTH_API(Crypto, size_t, BIO_ctrl_pending, (const char *file, const int line, const char *func, BIO * bio), (file, line, func, bio), (const char *, const int, const char *, BIO *), (_, _, _, _), (BIO * bio), (bio), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_write)
MOCK_OPENSSL_AUTH_API(Crypto, int, BIO_write, (const char *file, const int line, const char *func, BIO * bio, const void * data, int length), (file, line, func, bio, data, length), (const char *, const int, const char *, BIO *, const void *, int), (_, _, _, _, _, _), (BIO * bio, const void * data, int length), (bio, data, length), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_BIO_read)
MOCK_OPENSSL_AUTH_API(Crypto, int, BIO_read, (const char *file, const int line, const char *func, BIO * bio, void * data, int length), (file, line, func, bio, data, length), (const char *, const int, const char *, BIO *, void *, int), (_, _, _, _, _, _), (BIO * bio, void * data, int length), (bio, data, length), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_PEM_read_bio_X509)
MOCK_OPENSSL_AUTH_API(Crypto, X509 *, PEM_read_bio_X509, (const char *file, const int line, const char *func, BIO * bio, X509 ** certificate, pem_password_cb * callback, void * userdata), (file, line, func, bio, certificate, callback, userdata), (const char *, const int, const char *, BIO *, X509 **, pem_password_cb *, void *), (_, _, _, _, _, _, _), (BIO * bio, X509 ** certificate, pem_password_cb * callback, void * userdata), (bio, certificate, callback, userdata), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_PEM_read_bio_X509_CRL)
MOCK_OPENSSL_AUTH_API(Crypto, X509_CRL *, PEM_read_bio_X509_CRL, (const char *file, const int line, const char *func, BIO * bio, X509_CRL ** revocations, pem_password_cb * callback, void * userdata), (file, line, func, bio, revocations, callback, userdata), (const char *, const int, const char *, BIO *, X509_CRL **, pem_password_cb *, void *), (_, _, _, _, _, _, _), (BIO * bio, X509_CRL ** revocations, pem_password_cb * callback, void * userdata), (bio, revocations, callback, userdata), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_free)
MOCK_OPENSSL_AUTH_API(Crypto, void, X509_free, (const char *file, const int line, const char *func, X509 * certificate), (file, line, func, certificate), (const char *, const int, const char *, X509 *), (_, _, _, _), (X509 * certificate), (certificate), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_CRL_free)
MOCK_OPENSSL_AUTH_API(Crypto, void, X509_CRL_free, (const char *file, const int line, const char *func, X509_CRL * revocations), (file, line, func, revocations), (const char *, const int, const char *, X509_CRL *), (_, _, _, _), (X509_CRL * revocations), (revocations), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_STORE_add_crl)
MOCK_OPENSSL_AUTH_API(Crypto, int, X509_STORE_add_crl, (const char *file, const int line, const char *func, X509_STORE * store, X509_CRL * revocations), (file, line, func, store, revocations), (const char *, const int, const char *, X509_STORE *, X509_CRL *), (_, _, _, _, _), (X509_STORE * store, X509_CRL * revocations), (store, revocations), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_STORE_set_flags)
MOCK_OPENSSL_AUTH_API(Crypto, int, X509_STORE_set_flags, (const char *file, const int line, const char *func, X509_STORE * store, unsigned long flags), (file, line, func, store, flags), (const char *, const int, const char *, X509_STORE *, unsigned long), (_, _, _, _, _), (X509_STORE * store, unsigned long flags), (store, flags), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_get_ext_d2i)
MOCK_OPENSSL_AUTH_API(Crypto, void *, X509_get_ext_d2i, (const char *file, const int line, const char *func, const X509 * certificate, int identifier, int * critical, int * index), (file, line, func, certificate, identifier, critical, index), (const char *, const int, const char *, const X509 *, int, int *, int *), (_, _, _, _, _, _, _), (const X509 * certificate, int identifier, int * critical, int * index), (certificate, identifier, critical, index), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_GENERAL_NAMES_free)
MOCK_OPENSSL_AUTH_API(Crypto, void, GENERAL_NAMES_free, (const char *file, const int line, const char *func, GENERAL_NAMES * names), (file, line, func, names), (const char *, const int, const char *, GENERAL_NAMES *), (_, _, _, _), (GENERAL_NAMES * names), (names), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_OPENSSL_sk_num)
MOCK_OPENSSL_AUTH_API(Crypto, int, OPENSSL_sk_num, (const char *file, const int line, const char *func, const OPENSSL_STACK * stack), (file, line, func, stack), (const char *, const int, const char *, const OPENSSL_STACK *), (_, _, _, _), (const OPENSSL_STACK * stack), (stack), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_OPENSSL_sk_value)
MOCK_OPENSSL_AUTH_API(Crypto, void *, OPENSSL_sk_value, (const char *file, const int line, const char *func, const OPENSSL_STACK * stack, int index), (file, line, func, stack, index), (const char *, const int, const char *, const OPENSSL_STACK *, int), (_, _, _, _, _), (const OPENSSL_STACK * stack, int index), (stack, index), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_ASN1_STRING_length)
MOCK_OPENSSL_AUTH_API(Crypto, int, ASN1_STRING_length, (const char *file, const int line, const char *func, const ASN1_STRING * string), (file, line, func, string), (const char *, const int, const char *, const ASN1_STRING *), (_, _, _, _), (const ASN1_STRING * string), (string), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_ASN1_STRING_get0_data)
MOCK_OPENSSL_AUTH_API(Crypto, const unsigned char *, ASN1_STRING_get0_data, (const char *file, const int line, const char *func, const ASN1_STRING * string), (file, line, func, string), (const char *, const int, const char *, const ASN1_STRING *), (_, _, _, _), (const ASN1_STRING * string), (string), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_STORE_CTX_get_ex_data)
MOCK_OPENSSL_AUTH_API(Crypto, void *, X509_STORE_CTX_get_ex_data, (const char *file, const int line, const char *func, const X509_STORE_CTX * store, int index), (file, line, func, store, index), (const char *, const int, const char *, const X509_STORE_CTX *, int), (_, _, _, _, _), (const X509_STORE_CTX * store, int index), (store, index), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_STORE_CTX_get0_cert)
MOCK_OPENSSL_AUTH_API(Crypto, X509 *, X509_STORE_CTX_get0_cert, (const char *file, const int line, const char *func, const X509_STORE_CTX * store), (file, line, func, store), (const char *, const int, const char *, const X509_STORE_CTX *), (_, _, _, _), (const X509_STORE_CTX * store), (store), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_STORE_CTX_set_error)
MOCK_OPENSSL_AUTH_API(Crypto, void, X509_STORE_CTX_set_error, (const char *file, const int line, const char *func, X509_STORE_CTX * store, int error), (file, line, func, store, error), (const char *, const int, const char *, X509_STORE_CTX *, int), (_, _, _, _, _), (X509_STORE_CTX * store, int error), (store, error), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_STORE_CTX_get_error)
MOCK_OPENSSL_AUTH_API(Crypto, int, X509_STORE_CTX_get_error, (const char *file, const int line, const char *func, const X509_STORE_CTX * store), (file, line, func, store), (const char *, const int, const char *, const X509_STORE_CTX *), (_, _, _, _), (const X509_STORE_CTX * store), (store), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_verify_cert)
MOCK_OPENSSL_AUTH_API(Crypto, int, X509_verify_cert, (const char *file, const int line, const char *func, X509_STORE_CTX * store), (file, line, func, store), (const char *, const int, const char *, X509_STORE_CTX *), (_, _, _, _), (X509_STORE_CTX * store), (store), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_cmp_current_time)
MOCK_OPENSSL_AUTH_API(Crypto, int, X509_cmp_current_time, (const char *file, const int line, const char *func, const ASN1_TIME * time), (file, line, func, time), (const char *, const int, const char *, const ASN1_TIME *), (_, _, _, _), (const ASN1_TIME * time), (time), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_get0_notBefore)
MOCK_OPENSSL_AUTH_API(Crypto, const ASN1_TIME *, X509_get0_notBefore, (const char *file, const int line, const char *func, const X509 * certificate), (file, line, func, certificate), (const char *, const int, const char *, const X509 *), (_, _, _, _), (const X509 * certificate), (certificate), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_get0_notAfter)
MOCK_OPENSSL_AUTH_API(Crypto, const ASN1_TIME *, X509_get0_notAfter, (const char *file, const int line, const char *func, const X509 * certificate), (file, line, func, certificate), (const char *, const int, const char *, const X509 *), (_, _, _, _), (const X509 * certificate), (certificate), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_X509_cmp)
MOCK_OPENSSL_AUTH_API(Crypto, int, X509_cmp, (const char *file, const int line, const char *func, const X509 * left, const X509 * right), (file, line, func, left, right), (const char *, const int, const char *, const X509 *, const X509 *), (_, _, _, _, _), (const X509 * left, const X509 * right), (left, right), VALUE)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_ERR_clear_error)
MOCK_OPENSSL_AUTH_API(Crypto, void, ERR_clear_error, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VOID)
#endif

#if !defined(MOCK_OPENSSL_AUTH_ONE_FUNCTION) || defined(MOCK_OPENSSL_AUTH_FUNCTION_ERR_peek_last_error)
MOCK_OPENSSL_AUTH_API(Crypto, unsigned long, ERR_peek_last_error, (const char *file, const int line, const char *func), (file, line, func), (const char *, const int, const char *), (_, _, _), (void), (), VALUE)
#endif
