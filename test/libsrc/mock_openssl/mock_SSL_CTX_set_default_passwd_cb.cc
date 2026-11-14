#include <test_com.h>
#include <sharedLibrary.h>
#include <mock_openssl.h>

#define MOCK_OPENSSL_AUTH_ONE_FUNCTION
#define MOCK_OPENSSL_AUTH_FUNCTION_SSL_CTX_set_default_passwd_cb
#include "mock_openssl_auth_implementation.h"
