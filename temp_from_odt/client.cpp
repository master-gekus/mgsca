#include "client_p.h"

#include <cassert>

#include "openssl/ssl.h"
#include "openssl/x509.h"
#include "openssl/ec.h"

#include "odtunnel/settings.h"
#include "openssl_wrappers.h"
#include "firebase_p.h"

// Windows crypto.h defines X509_NAME as integer constant. We don't use Windows crypto.h!
#ifdef X509_NAME
# undef X509_NAME
#endif

namespace {
constexpr const TCHAR* odvpn_settings_client_id_key = _T("Client ID");
constexpr const TCHAR* odvpn_settings_client_cert_digest_key = _T("Client Certificate Digest");
constexpr const TCHAR* odvpn_settings_client_cert_key = _T("Client Certificate");
constexpr const TCHAR* odvpn_settings_client_private_key_key = _T("Client Private Key");

::odtunnel::string _get_name_component_by_nid(::X509_NAME *name, int nid)
{
  if (nullptr == name) {
    return ::odtunnel::string();
  }
  int pos = ::X509_NAME_get_index_by_NID(name, nid, -1);
  if (0 > pos) {
    return ::odtunnel::string();
  }
  ::X509_NAME_ENTRY *e = ::X509_NAME_get_entry(name, pos);
  if (nullptr == e) {
    return ::odtunnel::string();
  }
  ASN1_STRING *s = ::X509_NAME_ENTRY_get_data(e);
  if (nullptr == e) {
    return ::odtunnel::string();
  }

  unsigned char* utf8;
  int size = ::ASN1_STRING_to_UTF8(&utf8, s);
  if (0 > size) {
    return ::odtunnel::string();
  }

  return ::odtunnel::widen(std::string(static_cast<const char*>(static_cast<void*>(utf8)),
                                       static_cast<size_t>(size)));
}

bool _is_cert_valid()
{
  using namespace ::odtunnel::client;
  if (prv::client_id.empty() || prv::client_cert_digest.empty() || prv::client_cert_asn1.empty()
      || prv::client_private_key_asn1.empty()) {
    return false;
  }

  // Check sertificate valid.
  const unsigned char *buf = static_cast<const unsigned char *>(static_cast<const void*>(prv::client_cert_asn1.data()));
  ::odtunnel::openssl::object_ptr<::X509> c(::d2i_X509(nullptr, &buf, static_cast<int>(prv::client_cert_asn1.size())));
  if (!c) {
    return false;
  }

  // Check the private key corresponds to the certificate.
  ::odtunnel::openssl::object_ptr<SSL_CTX> ctx(SSL_CTX_new(TLS_method()));
  if (1 != SSL_CTX_use_certificate(ctx, c)) {
    return false;
  }

  buf = static_cast<const unsigned char *>(static_cast<const void*>(prv::client_private_key_asn1.data()));
  if (1 != SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_EC, ctx, buf, static_cast<int>(prv::client_private_key_asn1.size()))) {
    return false;
  }

  if (1 != SSL_CTX_check_private_key(ctx)) {
    return false;
  }

  // Check subject common name
  if (prv::client_id != _get_name_component_by_nid(X509_get_subject_name(c), NID_commonName)) {
    return false;
  }

  // Check prv::client_cert_digest
  if (prv::client_cert_digest != prv::_get_digest(c)) {
    return false;
  }

  return true;
}

::odtunnel::openssl::object_ptr<EC_KEY> _generate_ec_key(int nid)
{
  ::odtunnel::openssl::object_ptr<EC_KEY> key(::EC_KEY_new_by_curve_name(nid));
  EC_KEY_set_asn1_flag(key, OPENSSL_EC_NAMED_CURVE);
  EC_KEY_generate_key(key);
  return key;
}

} // namespace

namespace odtunnel {
namespace client {
namespace prv {
::odtunnel::string client_id;
::std::string client_cert_digest;
::std::string client_cert_asn1;
::std::string client_private_key_asn1;

::std::string _get_digest(::X509 *cert)
{
  unsigned char md[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  X509_digest(cert, EVP_sha1(), md, &md_len);
  return std::string(static_cast<const char*>(static_cast<void*>(md)), static_cast<size_t>(md_len));
}

::std::string _get_pubkey_digest(::X509 *cert)
{
  unsigned char md[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  X509_pubkey_digest(cert, EVP_sha1(), md, &md_len);
  return std::string(static_cast<const char*>(static_cast<void*>(md)), static_cast<size_t>(md_len));
}

std::string _mem_bio_to_string(BIO *bio)
{
  BUF_MEM *m;
  BIO_get_mem_ptr(bio, &m);
  return std::string(m->data, m->length);
}

} // namespace prv

void check_certificate(bool force_renew)
{
  if ((!force_renew) && (!_is_cert_valid())) {
    prv::client_id = settings::get_string(odvpn_settings_client_id_key);
    prv::client_cert_digest = settings::get_binary(odvpn_settings_client_cert_digest_key);
    prv::client_cert_asn1 = settings::get_binary(odvpn_settings_client_cert_key);
    prv::client_private_key_asn1 = settings::get_binary(odvpn_settings_client_private_key_key);
    force_renew = !_is_cert_valid();
  }

  if (force_renew) {
    prv::client_id = prv::_new_client_id();
    ::odtunnel::openssl::object_ptr<EC_KEY> key = _generate_ec_key(NID_X9_62_prime256v1);
    ::odtunnel::openssl::object_ptr<EVP_PKEY> evp_key(::EVP_PKEY_new());
    ::EVP_PKEY_set1_EC_KEY(evp_key, key);
    ::odtunnel::openssl::object_ptr<X509> cert(::X509_new());
    ::X509_set_pubkey(cert, evp_key);
    ::X509_set_version(cert, 2);
    ::ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
    ::X509_gmtime_adj(::X509_getm_notBefore(cert), 0);
    ::X509_gmtime_adj(::X509_getm_notAfter(cert), 60 * 60 * 24 * 365);
    X509_NAME *name = X509_get_subject_name(cert);
    X509_NAME_add_entry_by_txt(name, "C", V_ASN1_PRINTABLESTRING, (const unsigned char*)
                               ::odtunnel::narrow(::odtunnel::firebase::prv::current_region_).c_str(),  -1, -1, 0 );
    X509_NAME_add_entry_by_txt(name, "CN", V_ASN1_UTF8STRING, (const unsigned char*)
                               ::odtunnel::narrow(prv::client_id).c_str(), -1, -1, 0 );
    ::X509_set_issuer_name(cert, name);

    if (!::X509_sign(cert, evp_key, EVP_sha256())) {
      assert(false);
      prv::client_id.clear();
      prv::client_cert_digest.clear();
      prv::client_cert_asn1.clear();
      prv::client_private_key_asn1.clear();
      return;
    }

    ::odtunnel::openssl::object_ptr<BIO> cert_bio(::BIO_new(::BIO_s_mem()));
    ::odtunnel::openssl::object_ptr<BIO> pkey_bio(::BIO_new(::BIO_s_mem()));
    ::i2d_X509_bio(cert_bio, cert);
    ::i2d_ECPrivateKey_bio(pkey_bio, key);
    prv::client_cert_digest = prv::_get_digest(cert);
    prv::client_cert_asn1 = prv::_mem_bio_to_string(cert_bio);
    prv::client_private_key_asn1 = prv::_mem_bio_to_string(pkey_bio);

    if (!_is_cert_valid()) {
      assert(false);
      prv::client_id.clear();
      prv::client_cert_digest.clear();
      prv::client_cert_asn1.clear();
      prv::client_private_key_asn1.clear();
      return;
    }

    ::odtunnel::settings::set_string(odvpn_settings_client_id_key, prv::client_id);
    ::odtunnel::settings::set_binary(odvpn_settings_client_cert_key, prv::client_cert_asn1);
    ::odtunnel::settings::set_binary(odvpn_settings_client_cert_digest_key, prv::client_cert_digest);
    ::odtunnel::settings::set_binary(odvpn_settings_client_private_key_key, prv::client_private_key_asn1);
  }
}

::odtunnel::string get_id()
{
  check_certificate();
  return prv::client_id;
}

::std::string get_certificate()
{
  check_certificate();
  return prv::client_cert_asn1;
}

::std::string get_private_key()
{
  check_certificate();
  return prv::client_private_key_asn1;
}

} // namespace client
} // namespace odtunnel
