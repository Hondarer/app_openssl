#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

void delegate_real_EVP_CIPHER_CTX_free(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn = reinterpret_cast<decltype(&EVP_CIPHER_CTX_free)>(
        resolveSharedSymbolOrExit(kLibCryptoName, "EVP_CIPHER_CTX_free"));
    real_fn(ctx);
}

void mock_EVP_CIPHER_CTX_free(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx)
{
    if (_mock_openssl != nullptr)
    {
        _mock_openssl->EVP_CIPHER_CTX_free(file, line, func, ctx);
    }
    else
    {
        delegate_real_EVP_CIPHER_CTX_free(file, line, func, ctx);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_CIPHER_CTX_free");
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" from %s:%d\n", file, line);
        }
        else
        {
            printf("\n");
        }
    }
}
