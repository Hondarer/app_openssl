#ifndef _OVERRIDE_OPENSSL_RAND_H
#define _OVERRIDE_OPENSSL_RAND_H

/*
 * OpenSSL の実ヘッダーが相互に別のヘッダーを include する場合は、
 * 実ヘッダーの読み込みが完了するまでモックの注入を遅延する。
 */
#if defined(_IN_OVERRIDE_OPENSSL_REAL_HEADERS)
    #include_next <openssl/rand.h>
#else
    #define _IN_OVERRIDE_OPENSSL_REAL_HEADERS
    #include_next <openssl/rand.h>
    #undef _IN_OVERRIDE_OPENSSL_REAL_HEADERS

    /* モックにすげ替え */
    #define _IN_OVERRIDE_HEADER_OPENSSL_H
    #include <mock_openssl.h>
    #undef _IN_OVERRIDE_HEADER_OPENSSL_H
#endif

#endif // _OVERRIDE_OPENSSL_RAND_H
