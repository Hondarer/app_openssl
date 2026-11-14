#ifndef MOCK_OPENSSL_AUTH_IMPLEMENTATION_H
#define MOCK_OPENSSL_AUTH_IMPLEMENTATION_H

#ifndef MOCK_OPENSSL_AUTH_ONE_FUNCTION
    #error "Select one function before including the implementation"
#endif

#define MOCK_OPENSSL_AUTH_TRACE(name) \
    if (testing::getTraceLevel() > testing::TRACE_NONE) \
    { \
        printf("  > " #name); \
        if (testing::getTraceLevel() >= testing::TRACE_DETAIL) \
        { \
            printf(" from %s:%d\n", file, line); \
        } \
        else \
        { \
            printf("\n"); \
        } \
    }

#define MOCK_OPENSSL_AUTH_BODY_VALUE(result, name, arguments) \
    result mock_ret; \
    if (_mock_openssl != nullptr) \
    { \
        mock_ret = _mock_openssl->name arguments; \
    } \
    else \
    { \
        mock_ret = delegate_real_##name arguments; \
    } \
    MOCK_OPENSSL_AUTH_TRACE(name) \
    return mock_ret;

#define MOCK_OPENSSL_AUTH_BODY_VOID(result, name, arguments) \
    if (_mock_openssl != nullptr) \
    { \
        _mock_openssl->name arguments; \
    } \
    else \
    { \
        delegate_real_##name arguments; \
    } \
    MOCK_OPENSSL_AUTH_TRACE(name)

#define MOCK_OPENSSL_AUTH_API(library, result, name, parameters, arguments, types, matchers, real_parameters, real_arguments, kind) \
    result delegate_real_##name parameters \
    { \
        (void)file; \
        (void)line; \
        (void)func; \
        static auto real_fn = reinterpret_cast<decltype(&name)>(testing::resolveSharedSymbolOrExit(kLib##library##Name, #name)); \
        return real_fn real_arguments; \
    } \
    result mock_##name parameters \
    { \
        MOCK_OPENSSL_AUTH_BODY_##kind(result, name, arguments) \
    }

#include <mock_openssl_auth_api_table.h>

#undef MOCK_OPENSSL_AUTH_API
#undef MOCK_OPENSSL_AUTH_BODY_VOID
#undef MOCK_OPENSSL_AUTH_BODY_VALUE
#undef MOCK_OPENSSL_AUTH_TRACE

#endif // MOCK_OPENSSL_AUTH_IMPLEMENTATION_H
