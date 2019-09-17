#include "openssl_wrappers.h"

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

namespace odtunnel {
namespace openssl {

template<>
void object_free<::EC_KEY>::operator()(::EC_KEY* p) const noexcept
{
  ::EC_KEY_free(p);
}

template<>
void object_free<::BIO>::operator()(::BIO* p) const noexcept
{
  ::BIO_free_all(p);
}

template<>
void object_free<::EVP_PKEY>::operator()(::EVP_PKEY* p) const noexcept
{
  ::EVP_PKEY_free(p);
}

template<>
void object_free<::X509>::operator()(::X509* p) const noexcept
{
  ::X509_free(p);
}

template<>
void object_free<::SSL>::operator()(::SSL* p) const noexcept
{
  ::SSL_free(p);
}

template<>
void object_free<::SSL_CTX>::operator()(::SSL_CTX* p) const noexcept
{
  ::SSL_CTX_free(p);
}

} // namespace openssl
} // namespace odtunnel
