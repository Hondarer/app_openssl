#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_DecryptFinal_ex(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx,
                                      unsigned char *out, int *outl)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn = reinterpret_cast<decltype(&EVP_DecryptFinal_ex)>(
        resolveSharedSymbolOrExit(kLibCryptoName, "EVP_DecryptFinal_ex"));
    return real_fn(ctx, out, outl);
}

int mock_EVP_DecryptFinal_ex(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx,
                             unsigned char *out, int *outl)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_DecryptFinal_ex(file, line, func, ctx, out, outl);
    }
    else
    {
        mock_ret = delegate_real_EVP_DecryptFinal_ex(file, line, func, ctx, out, outl);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_DecryptFinal_ex");
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
