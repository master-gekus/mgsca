#include "odtunnel/firebase.h"
#include "firebase_p.h"

#include <langinfo.h>

#include "odtunnel/odtunnel.h"
#include "odtunnel/odtunnel_port.h"

namespace odtunnel {
namespace firebase {
namespace prv {

::odtunnel::firebase::result _http_request(bool use_ssl, ::odtunnel::string& response, const std::string& method,
                                           const std::string&  host, const std::string& path,
                                           const std::string& data_json)
{
#pragma message("TODO: Need implementation!")
  return ::odtunnel::firebase::result(::odtunnel::firebase::result::NotImplemented);
}

} // namespace prv

::odtunnel::string system_region()
{
#pragma message("TODO: Need implementation!")
  return _T("US");
}

} // namespace firebase
} // namespace odtunnel

