#include <mock_instance.h>
#include <mock_openssl.h>

using namespace testing;

Mock_openssl *_mock_openssl = nullptr;

Mock_openssl::Mock_openssl()
{
    ON_CALL(*this, EVP_CIPHER_CTX_new(_, _, _)).WillByDefault(Invoke(delegate_real_EVP_CIPHER_CTX_new));
    ON_CALL(*this, EVP_EncryptInit_ex(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_EncryptInit_ex));
    ON_CALL(*this, EVP_DecryptInit_ex(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_DecryptInit_ex));
    ON_CALL(*this, EVP_EncryptUpdate(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_EncryptUpdate));
    ON_CALL(*this, EVP_DecryptUpdate(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_DecryptUpdate));
    ON_CALL(*this, EVP_EncryptFinal_ex(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_EncryptFinal_ex));
    ON_CALL(*this, EVP_DecryptFinal_ex(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_DecryptFinal_ex));
    ON_CALL(*this, EVP_CIPHER_CTX_ctrl(_, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_CIPHER_CTX_ctrl));
    ON_CALL(*this, EVP_MD_CTX_new(_, _, _)).WillByDefault(Invoke(delegate_real_EVP_MD_CTX_new));
    ON_CALL(*this, EVP_DigestInit_ex(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_DigestInit_ex));
    ON_CALL(*this, EVP_DigestUpdate(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_DigestUpdate));
    ON_CALL(*this, EVP_DigestFinal_ex(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_EVP_DigestFinal_ex));
    ON_CALL(*this, RAND_bytes(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_RAND_bytes));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_openssl);
}

Mock_openssl::~Mock_openssl()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_openssl);
}
