#include "firebase_p.h"

#include <ctime>
#include <limits>

#include "odtunnel/settings.h"
#include "odtunnel/client.h"

namespace {

const ::odtunnel::string odvpn_firebase_host{_T("native-vpn.firebaseio.com")};
const ::odtunnel::string odvpn_default_config{_T("//config/server.json")};
constexpr const char* google_api_host = "www.googleapis.com";
constexpr const char* google_new_user_request
  = "/identitytoolkit/v3/relyingparty/signupNewUser?key=AIzaSyADfT7FlaJhKlArkToY_yXpjckALy3P_PA";
constexpr const char* google_new_user_request_bosy = "{\"returnSecureToken\":true}";

constexpr const TCHAR* odvpn_settings_auth_key = _T("Firebase auth ID");
constexpr const TCHAR* odvpn_settings_local_id_key = _T("Firebase local ID");
constexpr const TCHAR* odvpn_settings_expiration_time_key = _T("Firebase expiration time");

const TCHAR* get_value_start(const TCHAR *source, const TCHAR* name)
{
  if (nullptr == name) {
    return nullptr;
  }
  size_t len = _tcslen(name);
  if (0 == len) {
    return nullptr;
  }

  TCHAR* qname = (TCHAR*)_alloca((len + 4) * sizeof(TCHAR));
  qname[0] = _T('"');
  memcpy(qname + 1, name, len * sizeof(TCHAR));
  qname[len + 1] = _T('"');
  qname[len + 2] = _T(':');
  qname[len + 3] = _T('\0');

  const TCHAR* s = _tcsstr(source, qname);
  if (nullptr == s) {
    return nullptr;
  }

  s += (len + 3);
  while (_T(' ') == (*s)) {
    ++s;
  }

  return s;
}

bool get_string_value(const TCHAR *source, const TCHAR* name, ::odtunnel::string& value)
{
  const TCHAR *start = get_value_start(source, name);
  if ((nullptr == start) || (_T('"') != (*start))) {
    return false;
  }
  ++start;
  const TCHAR *end = _tcschr(start, _T('"'));
  if (nullptr == end) {
    return false;
  }

  value.assign(start, end - start);
  return true;
}

bool get_int_value(const TCHAR* source, const TCHAR* name, intmax_t& value)
{
  const TCHAR *start = get_value_start(source, name);
  if (nullptr == start) {
    return false;
  }

  if (_T('"') == (*start)) {
    ++start;
  }

  TCHAR *end;
  intmax_t res = _tcstoimax(start, &end, 0);
  if (end == start) {
    return false;
  }
  value = res;
  return true;
}

::odtunnel::string firebase_auth;
::odtunnel::string firebase_local_id;
std::time_t firebase_expiration_time{0};

} // namespace

namespace odtunnel {
namespace firebase {
namespace prv {
::odtunnel::string current_region_{_T("US")};
::odtunnel::string target_region_{_T("US")};
} // namespace prv

const TCHAR* current_region()
{
  return prv::current_region_.c_str();
}

void set_current_region(const TCHAR* region)
{
  prv::current_region_ = region;
}

const TCHAR* target_region()
{
  return prv::target_region_.c_str();
}

void set_target_region(const TCHAR* region)
{
  prv::target_region_ = region;
}

result acquire_current_region()
{
  ::odtunnel::string json;
  result res;
  res = prv::_http_request(false, json, "GET", "ip-api.com", "/json");
  if (!res.is_ok()) {
    return res;
  }

  ::odtunnel::string country_code;
  if (!get_string_value(json.c_str(), _T("countryCode"), country_code)) {
    res.error_code_ = result::ParseError;
    res.error_message_ = _T("Parse error: countryCode not found.");
    return res;
  }

  prv::current_region_ = country_code;

  return res;
}

result get_default_server(std::string& host, uint16_t& port)
{
  ::odtunnel::string json;
  result res;
  res = prv::_http_request(true, json, "GET", narrow(odvpn_firebase_host), narrow(odvpn_default_config));
  if (!res.is_ok()) {
    return res;
  }

  ::odtunnel::string h;
  if (!get_string_value(json.c_str(), _T("address"), h)) {
    res.error_code_ = result::ParseError;
    res.error_message_ = _T("Parse error: Host address not found.");
    return res;
  }

  intmax_t p;
  if (!get_int_value(json.c_str(), _T("port"), p)) {
    res.error_code_ = result::ParseError;
    res.error_message_ = _T("Parse error: Port not found.");
    return res;
  }

  if ((0 > p) || (static_cast<intmax_t>(std::numeric_limits<uint16_t>::max()) < p)) {
    res.error_code_ = result::ParseError;
    res.error_message_ = _T("Parse error: Invalid port value.");
    return res;
  }

  host = narrow(h);
  port = static_cast<uint16_t>(p);
  return res;
}

result acquire_firebase_account(bool force)
{
  result res;

  if (!force) {
    if((0 == firebase_expiration_time) || firebase_auth.empty() || firebase_local_id.empty()) {
      firebase_auth = ::odtunnel::settings::get_value<::odtunnel::string>(odvpn_settings_auth_key);
      firebase_local_id = ::odtunnel::settings::get_value<::odtunnel::string>(odvpn_settings_local_id_key);
      firebase_expiration_time = ::odtunnel::settings::get_value<std::time_t>(odvpn_settings_expiration_time_key);
    }
    if ((std::time(nullptr) < firebase_expiration_time) && (!firebase_auth.empty()) && (!firebase_local_id.empty())) {
      res.error_message_ = _T("Valid FireBase ID already exists.");
      return res;
    }
  }

  ::odtunnel::string response;
  res = prv::_http_request(true, response, "POST", google_api_host, google_new_user_request,
                           google_new_user_request_bosy);
  if (!res.is_ok()) {
    return res;
  }

  intmax_t exp_timeout;
  ::odtunnel::string auth, localid;
  if ((!get_int_value(response.c_str(), _T("expiresIn"), exp_timeout))
      || (!get_string_value(response.c_str(), _T("localId"), localid))
      || (!get_string_value(response.c_str(), _T("idToken"), auth))) {
    res.error_code_ = result::ParseError;
    res.error_message_ = _T("Parse error: Can not acquire temporary FireBase user ID.");
    return res;
  }

  firebase_auth = auth;
  firebase_local_id = localid;
  firebase_expiration_time = static_cast<std::time_t>(std::time(nullptr) + exp_timeout - 60);
  ::odtunnel::settings::set_value(odvpn_settings_auth_key, firebase_auth);
  ::odtunnel::settings::set_value(odvpn_settings_local_id_key, firebase_local_id);
  ::odtunnel::settings::set_value(odvpn_settings_expiration_time_key, firebase_expiration_time);

  res.error_message_ = _T("FireBase ID successfully acquired.");
  return res;
}

namespace {
inline std::string _firebase_get_server_request_path()
{
  return "/requests_v2/" + narrow(firebase_local_id) + "/GET_SERVER/NEW_REQUEST.json?auth=" + narrow(firebase_auth);
}

inline std::string _firebase_get_server_request_json()
{
  return "{\"clientId\":\"" + narrow(::odtunnel::client::get_id()) + "\",\"region\":\""
      + narrow(prv::current_region_) + "\",\"target_region\":\"" + narrow(prv::target_region_) + "\"}";
}

inline bool _firebase_get_server_request()
{
  ::odtunnel::string response;
  return prv::_http_request(true, response, "PUT", narrow(odvpn_firebase_host), _firebase_get_server_request_path(),
                            _firebase_get_server_request_json()).is_ok();
}

inline std::string _firebase_get_server_response_path()
{
  return "/requests_v2/" + narrow(firebase_local_id) + "/GET_SERVER/DONE.json?auth=" + narrow(firebase_auth);
}

inline bool _firebase_get_server_response(std::string& host, uint16_t& port)
{
  ::odtunnel::string response;
  if (!prv::_http_request(true, response, "GET", narrow(odvpn_firebase_host),
                          _firebase_get_server_response_path()).is_ok()) {
    return false;
  }

  ::odtunnel::string h;
  intmax_t p;
  if ((!get_string_value(response.c_str(), _T("address"), h))
      || (!get_int_value(response.c_str(), _T("port"), p)) || (1 > p) || (65535 < p)) {
    return false;
  }
  host = narrow(h);
  port = static_cast<uint16_t>(p);

  return true;
}
} // namespace

result get_server(std::string& host, uint16_t& port)
{
  if ((!acquire_firebase_account().is_ok())
      || (!_firebase_get_server_request()) || (!_firebase_get_server_response(host, port))) {
    return get_default_server(host, port);
  }

  return result{};
}

} // namespace firebase
} // namespace odtunnel

