#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

EVP_CIPHER_CTX *delegate_real_EVP_CIPHER_CTX_new(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn = reinterpret_cast<decltype(&EVP_CIPHER_CTX_new)>(
        resolveSharedSymbolOrExit(kLibCryptoName, "EVP_CIPHER_CTX_new"));
    return real_fn();
}

EVP_CIPHER_CTX *mock_EVP_CIPHER_CTX_new(const char *file, const int line, const char *func)
{
    EVP_CIPHER_CTX *mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_CIPHER_CTX_new(file, line, func);
    }
    else
    {
        mock_ret = delegate_real_EVP_CIPHER_CTX_new(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_CIPHER_CTX_new");
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
