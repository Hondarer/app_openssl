#ifndef _MOCK_OPENSSL_H
#define _MOCK_OPENSSL_H

/* override 経由の場合、OpenSSL のヘッダーは include_override 側で取り込み済みである。
 * ここで再取り込みすると、もう一方の override ヘッダーが
 * _IN_OVERRIDE_HEADER_OPENSSL_H を #undef して差し替えが無効になる。 */
#ifndef _IN_OVERRIDE_HEADER_OPENSSL_H
    #include <openssl/evp.h>
    #include <openssl/rand.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    extern EVP_CIPHER_CTX *mock_EVP_CIPHER_CTX_new(const char *, const int, const char *);
    extern int mock_EVP_EncryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                       ENGINE *, const unsigned char *, const unsigned char *);
    extern int mock_EVP_DecryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                       ENGINE *, const unsigned char *, const unsigned char *);
    extern int mock_EVP_EncryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *,
                                      const unsigned char *, int);
    extern int mock_EVP_DecryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *,
                                      const unsigned char *, int);
    extern int mock_EVP_EncryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                        int *);
    extern int mock_EVP_DecryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                        int *);
    extern int mock_EVP_CIPHER_CTX_ctrl(const char *, const int, const char *, EVP_CIPHER_CTX *, int, int, void *);
    extern EVP_MD_CTX *mock_EVP_MD_CTX_new(const char *, const int, const char *);
    extern int mock_EVP_DigestInit_ex(const char *, const int, const char *, EVP_MD_CTX *, const EVP_MD *, ENGINE *);
    extern int mock_EVP_DigestUpdate(const char *, const int, const char *, EVP_MD_CTX *, const void *, size_t);
    extern int mock_EVP_DigestFinal_ex(const char *, const int, const char *, EVP_MD_CTX *, unsigned char *,
                                       unsigned int *);
    extern int mock_RAND_bytes(const char *, const int, const char *, unsigned char *, int);

#ifdef __cplusplus
}
#endif

#ifdef _IN_OVERRIDE_HEADER_OPENSSL_H

    #define EVP_CIPHER_CTX_new() mock_EVP_CIPHER_CTX_new(__FILE__, __LINE__, __func__)
    #define EVP_EncryptInit_ex(ctx, type, impl, key, iv) \
        mock_EVP_EncryptInit_ex(__FILE__, __LINE__, __func__, ctx, type, impl, key, iv)
    #define EVP_DecryptInit_ex(ctx, type, impl, key, iv) \
        mock_EVP_DecryptInit_ex(__FILE__, __LINE__, __func__, ctx, type, impl, key, iv)
    #define EVP_EncryptUpdate(ctx, out, outl, in, inl) \
        mock_EVP_EncryptUpdate(__FILE__, __LINE__, __func__, ctx, out, outl, in, inl)
    #define EVP_DecryptUpdate(ctx, out, outl, in, inl) \
        mock_EVP_DecryptUpdate(__FILE__, __LINE__, __func__, ctx, out, outl, in, inl)
    #define EVP_EncryptFinal_ex(ctx, out, outl) mock_EVP_EncryptFinal_ex(__FILE__, __LINE__, __func__, ctx, out, outl)
    #define EVP_DecryptFinal_ex(ctx, out, outl) mock_EVP_DecryptFinal_ex(__FILE__, __LINE__, __func__, ctx, out, outl)
    #define EVP_CIPHER_CTX_ctrl(ctx, type, arg, ptr) \
        mock_EVP_CIPHER_CTX_ctrl(__FILE__, __LINE__, __func__, ctx, type, arg, ptr)
    #define EVP_MD_CTX_new()                   mock_EVP_MD_CTX_new(__FILE__, __LINE__, __func__)
    #define EVP_DigestInit_ex(ctx, type, impl) mock_EVP_DigestInit_ex(__FILE__, __LINE__, __func__, ctx, type, impl)
    #define EVP_DigestUpdate(ctx, d, cnt)      mock_EVP_DigestUpdate(__FILE__, __LINE__, __func__, ctx, d, cnt)
    #define EVP_DigestFinal_ex(ctx, md, s)     mock_EVP_DigestFinal_ex(__FILE__, __LINE__, __func__, ctx, md, s)
    #define RAND_bytes(buf, num)               mock_RAND_bytes(__FILE__, __LINE__, __func__, buf, num)

#else // _IN_OVERRIDE_HEADER_OPENSSL_H

    #include <gmock/gmock.h>

extern EVP_CIPHER_CTX *delegate_real_EVP_CIPHER_CTX_new(const char *, const int, const char *);
extern int delegate_real_EVP_EncryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                            ENGINE *, const unsigned char *, const unsigned char *);
extern int delegate_real_EVP_DecryptInit_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *,
                                            ENGINE *, const unsigned char *, const unsigned char *);
extern int delegate_real_EVP_EncryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                           int *, const unsigned char *, int);
extern int delegate_real_EVP_DecryptUpdate(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                           int *, const unsigned char *, int);
extern int delegate_real_EVP_EncryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                             int *);
extern int delegate_real_EVP_DecryptFinal_ex(const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *,
                                             int *);
extern int delegate_real_EVP_CIPHER_CTX_ctrl(const char *, const int, const char *, EVP_CIPHER_CTX *, int, int, void *);
extern EVP_MD_CTX *delegate_real_EVP_MD_CTX_new(const char *, const int, const char *);
extern int delegate_real_EVP_DigestInit_ex(const char *, const int, const char *, EVP_MD_CTX *, const EVP_MD *,
                                           ENGINE *);
extern int delegate_real_EVP_DigestUpdate(const char *, const int, const char *, EVP_MD_CTX *, const void *, size_t);
extern int delegate_real_EVP_DigestFinal_ex(const char *, const int, const char *, EVP_MD_CTX *, unsigned char *,
                                            unsigned int *);
extern int delegate_real_RAND_bytes(const char *, const int, const char *, unsigned char *, int);

class Mock_openssl
{
  public:
    MOCK_METHOD(EVP_CIPHER_CTX *, EVP_CIPHER_CTX_new, (const char *, const int, const char *));
    MOCK_METHOD(int, EVP_EncryptInit_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                 const unsigned char *, const unsigned char *));
    MOCK_METHOD(int, EVP_DecryptInit_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, const EVP_CIPHER *, ENGINE *,
                 const unsigned char *, const unsigned char *));
    MOCK_METHOD(int, EVP_EncryptUpdate,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *, const unsigned char *,
                 int));
    MOCK_METHOD(int, EVP_DecryptUpdate,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *, const unsigned char *,
                 int));
    MOCK_METHOD(int, EVP_EncryptFinal_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *));
    MOCK_METHOD(int, EVP_DecryptFinal_ex,
                (const char *, const int, const char *, EVP_CIPHER_CTX *, unsigned char *, int *));
    MOCK_METHOD(int, EVP_CIPHER_CTX_ctrl, (const char *, const int, const char *, EVP_CIPHER_CTX *, int, int, void *));
    MOCK_METHOD(EVP_MD_CTX *, EVP_MD_CTX_new, (const char *, const int, const char *));
    MOCK_METHOD(int, EVP_DigestInit_ex,
                (const char *, const int, const char *, EVP_MD_CTX *, const EVP_MD *, ENGINE *));
    MOCK_METHOD(int, EVP_DigestUpdate, (const char *, const int, const char *, EVP_MD_CTX *, const void *, size_t));
    MOCK_METHOD(int, EVP_DigestFinal_ex,
                (const char *, const int, const char *, EVP_MD_CTX *, unsigned char *, unsigned int *));
    MOCK_METHOD(int, RAND_bytes, (const char *, const int, const char *, unsigned char *, int));

    Mock_openssl();
    virtual ~Mock_openssl();
};

extern Mock_openssl *_mock_openssl;

#endif // _IN_OVERRIDE_HEADER_OPENSSL_H

#endif // _MOCK_OPENSSL_H
