#ifndef _OVERRIDE_OPENSSL_RAND_H
#define _OVERRIDE_OPENSSL_RAND_H

/*
 * OpenSSL の実ヘッダーの取り込み方。
 * Linux は #include_next を使う。MSVC には #include_next がないため、Windows は
 * include パス (app/openssl/prod/include) からの相対パスで実ヘッダーを指定する。
 *
 * 実ヘッダーが別の OpenSSL のヘッダーを取り込む場合は、実ヘッダーの読み込みが完了するまで
 * mock の注入を遅延する。
 */
#if defined(_IN_OVERRIDE_OPENSSL_REAL_HEADERS)
    #ifndef _WIN32
        #include_next <openssl/rand.h>
    #else
        #include <../include/openssl/rand.h>
    #endif
#else
    /*
     * 置換マクロを定義する前に、mock が置き換える関数を宣言する実ヘッダーをすべて取り込む。
     * 置換マクロの定義後に実ヘッダーが取り込まれると、関数宣言がマクロで書き換えられる。
     */
    #define _IN_OVERRIDE_OPENSSL_REAL_HEADERS
    #ifndef _WIN32
        #include_next <openssl/evp.h>
        #include_next <openssl/rand.h>
    #else
        #include <../include/openssl/evp.h>
        #include <../include/openssl/rand.h>
    #endif
    #undef _IN_OVERRIDE_OPENSSL_REAL_HEADERS

    /* モックにすげ替え */
    #define _IN_OVERRIDE_HEADER_OPENSSL_H
    #include <mock_openssl.h>
    #undef _IN_OVERRIDE_HEADER_OPENSSL_H
#endif

#endif // _OVERRIDE_OPENSSL_RAND_H
