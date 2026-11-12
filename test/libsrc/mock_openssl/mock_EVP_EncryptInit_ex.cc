#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_EncryptInit_ex(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx,
                                     const EVP_CIPHER *type, ENGINE *impl, const unsigned char *key,
                                     const unsigned char *iv)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn = reinterpret_cast<decltype(&EVP_EncryptInit_ex)>(
        resolveSharedSymbolOrExit(kLibCryptoName, "EVP_EncryptInit_ex"));
    return real_fn(ctx, type, impl, key, iv);
}

int mock_EVP_EncryptInit_ex(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx,
                            const EVP_CIPHER *type, ENGINE *impl, const unsigned char *key, const unsigned char *iv)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_EncryptInit_ex(file, line, func, ctx, type, impl, key, iv);
    }
    else
    {
        mock_ret = delegate_real_EVP_EncryptInit_ex(file, line, func, ctx, type, impl, key, iv);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_EncryptInit_ex");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }

    return mock_ret;
}
