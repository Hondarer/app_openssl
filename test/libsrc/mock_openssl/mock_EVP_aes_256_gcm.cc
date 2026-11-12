#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

using namespace testing;

const EVP_CIPHER *delegate_real_EVP_aes_256_gcm(const char *file, const int line, const char *func)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    static auto real_fn =
        reinterpret_cast<decltype(&EVP_aes_256_gcm)>(resolveSharedSymbolOrExit(kLibCryptoName, "EVP_aes_256_gcm"));
    return real_fn();
}

const EVP_CIPHER *mock_EVP_aes_256_gcm(const char *file, const int line, const char *func)
{
    const EVP_CIPHER *mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_aes_256_gcm(file, line, func);
    }
    else
    {
        mock_ret = delegate_real_EVP_aes_256_gcm(file, line, func);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_aes_256_gcm");
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
