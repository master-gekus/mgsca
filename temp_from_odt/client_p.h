/**
 * @file settings_p.h
 * @author Evgeny V. Zalivochkin
 */

#pragma once
#ifndef _DIGITALORDER_SETTINGS_PRIVATE_H_INCLUDED_
#define _DIGITALORDER_SETTINGS_PRIVATE_H_INCLUDED_

#include "openssl/x509.h"

#include "odtunnel/client.h"

namespace odtunnel {
namespace client {
namespace prv {

extern ::odtunnel::string client_id;
extern ::std::string client_cert_digest;
extern ::std::string client_cert_asn1;
extern ::std::string client_private_key_asn1;

::odtunnel::string _new_client_id();
::std::string _get_digest(::X509 *cert);
::std::string _get_pubkey_digest(::X509 *cert);
::std::string _mem_bio_to_string(::BIO *bio);

} // namespace prv
} // namespace client
} // namespace odtunnel

#endif // _DIGITALORDER_SETTINGS_PRIVATE_H_INCLUDED_
