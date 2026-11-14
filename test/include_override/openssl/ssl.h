#ifndef _OVERRIDE_OPENSSL_SSL_H
#define _OVERRIDE_OPENSSL_SSL_H

#if defined(_IN_OVERRIDE_OPENSSL_REAL_HEADERS)
    #ifndef _WIN32
        #include_next <openssl/ssl.h>
    #else
        #include <../include/openssl/ssl.h>
    #endif
#else
    /* 関連する実ヘッダーの宣言をすべて読んでから置換する。 */
    #define _IN_OVERRIDE_OPENSSL_REAL_HEADERS
    #ifndef _WIN32
        #include_next <openssl/evp.h>
        #include_next <openssl/rand.h>
        #include_next <openssl/ssl.h>
        #include_next <openssl/x509.h>
        #include_next <openssl/x509_vfy.h>
        #include_next <openssl/x509v3.h>
        #include_next <openssl/pem.h>
        #include_next <openssl/bio.h>
        #include_next <openssl/err.h>
        #include_next <openssl/crypto.h>
    #else
        #include <../include/openssl/evp.h>
        #include <../include/openssl/rand.h>
        #include <../include/openssl/ssl.h>
        #include <../include/openssl/x509.h>
        #include <../include/openssl/x509_vfy.h>
        #include <../include/openssl/x509v3.h>
        #include <../include/openssl/pem.h>
        #include <../include/openssl/bio.h>
        #include <../include/openssl/err.h>
        #include <../include/openssl/crypto.h>
    #endif
    #undef _IN_OVERRIDE_OPENSSL_REAL_HEADERS

    #define _IN_OVERRIDE_HEADER_OPENSSL_H
    #include <mock_openssl.h>
    #undef _IN_OVERRIDE_HEADER_OPENSSL_H
#endif

#endif // _OVERRIDE_OPENSSL_SSL_H
