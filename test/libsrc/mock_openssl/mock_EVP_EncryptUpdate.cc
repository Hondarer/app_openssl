#include <test_com.h>
#include <mock_openssl.h>

using namespace testing;

int delegate_real_EVP_EncryptUpdate(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx,
                                    unsigned char *out, int *outl, const unsigned char *in, int inl)
{
    // avoid -Wunused-parameter
    (void)file;
    (void)line;
    (void)func;

    return EVP_EncryptUpdate(ctx, out, outl, in, inl);
}

int mock_EVP_EncryptUpdate(const char *file, const int line, const char *func, EVP_CIPHER_CTX *ctx, unsigned char *out,
                           int *outl, const unsigned char *in, int inl)
{
    int mock_ret;

    if (_mock_openssl != nullptr)
    {
        mock_ret = _mock_openssl->EVP_EncryptUpdate(file, line, func, ctx, out, outl, in, inl);
    }
    else
    {
        mock_ret = delegate_real_EVP_EncryptUpdate(file, line, func, ctx, out, outl, in, inl);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > EVP_EncryptUpdate");
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
