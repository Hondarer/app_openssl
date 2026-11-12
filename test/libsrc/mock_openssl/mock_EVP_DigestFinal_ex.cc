#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_DigestFinal_ex(const char *file, const int line, const char *func, EVP_MD_CTX *ctx,
                                     unsigned char *md, unsigned int *s)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn = reinterpret_cast<decltype(&EVP_DigestFinal_ex)>(
        resolveSharedSymbolOrExit(kLibCryptoName, "EVP_DigestFinal_ex"));
    return real_fn(ctx, md, s);
}

int mock_EVP_DigestFinal_ex(const char *file, const int line, const char *func, EVP_MD_CTX *ctx, unsigned char *md,
                            unsigned int *s)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_DigestFinal_ex(file, line, func, ctx, md, s);
    }
    else
    {
        mock_ret = delegate_real_EVP_DigestFinal_ex(file, line, func, ctx, md, s);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_DigestFinal_ex");
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
