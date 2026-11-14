#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

#define MOCK_OPENSSL_AUTH_ONE_FUNCTION
#define MOCK_OPENSSL_AUTH_FUNCTION_SSL_get_ex_data_X509_STORE_CTX_idx
#include "mock_openssl_auth_implementation.h"
