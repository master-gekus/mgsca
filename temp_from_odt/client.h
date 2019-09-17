/**
 * @file client.h
 * @author Evgeny V. Zalivochkin
 */

#pragma once
#ifndef _DIGITALORDER_CLIENT_H_INCLUDED_
#define _DIGITALORDER_CLIENT_H_INCLUDED_

#include "odtunnel/odtunnel_port.h"

namespace odtunnel {
namespace client {

void check_certificate(bool force_renew = false);
::odtunnel::string get_id();
::std::string get_certificate();
::std::string get_private_key();

} // namespace client
} // namespace odtunnel


#endif // _DIGITALORDER_CLIENT_H_INCLUDED_
