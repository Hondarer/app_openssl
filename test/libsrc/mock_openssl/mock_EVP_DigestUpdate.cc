#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_DigestUpdate(const char *file, const int line, const char *func, EVP_MD_CTX *ctx, const void *d,
                                   size_t cnt)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn =
        reinterpret_cast<decltype(&EVP_DigestUpdate)>(resolveSharedSymbolOrExit(kLibCryptoName, "EVP_DigestUpdate"));
    return real_fn(ctx, d, cnt);
}

int mock_EVP_DigestUpdate(const char *file, const int line, const char *func, EVP_MD_CTX *ctx, const void *d,
                          size_t cnt)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_DigestUpdate(file, line, func, ctx, d, cnt);
    }
    else
    {
        mock_ret = delegate_real_EVP_DigestUpdate(file, line, func, ctx, d, cnt);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_DigestUpdate");
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
