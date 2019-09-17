/**
 * @file openssl_wrappers.h
 * @author Evgeny V. Zalivochkin
 */

#pragma once
#ifndef _DIGITALORDER_OPENSSL_WRAPPERS_H_INCLUDED_
#define _DIGITALORDER_OPENSSL_WRAPPERS_H_INCLUDED_

#include <memory>

#include <openssl/ssl.h>

namespace odtunnel {
namespace openssl {

template<typename T> struct object_free
{
  constexpr object_free() noexcept = default;
  void operator()(T*) const noexcept;
};

template <typename T>
class object_ptr : public std::shared_ptr<T>
{
public:
  object_ptr() : std::shared_ptr<T>(nullptr, object_free<T>()) {}
  object_ptr(T* o) : std::shared_ptr<T>(o, object_free<T>()) {}
  object_ptr(const object_ptr&) = default;
  object_ptr(object_ptr&&) = default;
  object_ptr& operator =(const object_ptr&) = default;
  object_ptr& operator =(object_ptr&&) = default;
public:
  inline operator T* () {return this->get();}
  inline operator const T* () const {return this->get();}
};

} // namespace openssl
} // namespace odtunnel

#if OPENSSL_VERSION_NUMBER < 0x10100000L
inline const SSL_METHOD *TLS_method(void)
{
  return SSLv23_method();
}

inline const SSL_METHOD *TLS_server_method(void)
{
  return SSLv23_server_method();
}

inline const SSL_METHOD *TLS_client_method(void)
{
  return SSLv23_client_method();
}

inline ASN1_TIME *X509_getm_notBefore(const X509 *x)
{
  return X509_get_notBefore(x);
}

inline ASN1_TIME *X509_getm_notAfter(const X509 *x)
{
  return X509_get_notAfter(x);
}
#endif

#endif // _DIGITALORDER_OPENSSL_WRAPPERS_H_INCLUDED_
