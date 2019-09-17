/**
 * @file firebase.h
 * @author Evgeny V. Zalivochkin
 */

#pragma once
#ifndef _DIGITALORDER_FIREBASE_H_INCLUDED_
#define _DIGITALORDER_FIREBASE_H_INCLUDED_

#include <cinttypes>
#include <cstring>

#include "odtunnel/odtunnel_port.h"
#include "odtunnel/sockets_port.h"

namespace odtunnel {
namespace firebase {

struct result
{
  enum ErrorCode {
    NoError = 0,
    HTTPError = 1,
    ParseError = 2,
    NotImplemented = 255,
  };

  inline result() {}
  inline result(ErrorCode code) : error_code_(code) {}
  inline result(ErrorCode code, const string& message) : error_code_(code), error_message_(message) {}
  inline result(ErrorCode code, const TCHAR* message) : error_code_(code), error_message_(message) {}

  ErrorCode error_code_ = NoError;
  string error_message_;

  inline bool is_ok() const
  {
    return NoError == error_code_;
  }
};

const TCHAR* current_region();
void set_current_region(const TCHAR* region);
result acquire_current_region();
::odtunnel::string system_region();

const TCHAR* target_region();
void set_target_region(const TCHAR* region);

result acquire_firebase_account(bool force = false);
result get_default_server(std::string& host, uint16_t& port);
result get_server(std::string& host, uint16_t& port);

} // namespace firebase
} // namespace odtunnel

#endif // _DIGITALORDER_FIREBASE_H_INCLUDED_
