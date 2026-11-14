#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

#define MOCK_OPENSSL_AUTH_ONE_FUNCTION
#define MOCK_OPENSSL_AUTH_FUNCTION_OPENSSL_sk_value
#include "mock_openssl_auth_implementation.h"
