#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_DigestInit_ex(const char *file, const int line, const char *func, EVP_MD_CTX *ctx,
                                    const EVP_MD *type, ENGINE *impl)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn =
        reinterpret_cast<decltype(&EVP_DigestInit_ex)>(resolveSharedSymbolOrExit(kLibCryptoName, "EVP_DigestInit_ex"));
    return real_fn(ctx, type, impl);
}

int mock_EVP_DigestInit_ex(const char *file, const int line, const char *func, EVP_MD_CTX *ctx, const EVP_MD *type,
                           ENGINE *impl)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_DigestInit_ex(file, line, func, ctx, type, impl);
    }
    else
    {
        mock_ret = delegate_real_EVP_DigestInit_ex(file, line, func, ctx, type, impl);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_DigestInit_ex");
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
