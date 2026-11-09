#include <test_com.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_CIPHER_CTX_ctrl(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx, int type,
                                      int arg, void *ptr)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return EVP_CIPHER_CTX_ctrl(ctx, type, arg, ptr);
}

int mock_EVP_CIPHER_CTX_ctrl(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx, int type, int arg,
                             void *ptr)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_CIPHER_CTX_ctrl(file, line, func, ctx, type, arg, ptr);
    }
    else
    {
        mock_ret = delegate_real_EVP_CIPHER_CTX_ctrl(file, line, func, ctx, type, arg, ptr);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_CIPHER_CTX_ctrl");
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
