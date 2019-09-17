#include "odtunnel/firebase.h"
#include "firebase_p.h"

#include <Windows.h>
#include <Winhttp.h>

#include "odtunnel/odtunnel.h"

namespace odtunnel {
namespace firebase {
namespace prv {

::odtunnel::firebase::result _http_request(bool use_ssl, ::odtunnel::string& response, const std::string& method,
                                           const std::string&  host, const std::string& path,
                                           const std::string& data_json)
{
  HINTERNET session = NULL;
  HINTERNET connect = NULL;
  HINTERNET request = NULL;

  std::string resp;
  ::odtunnel::firebase::result result;

  try {
    session = ::WinHttpOpen(L"OD VPN Clienst/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (NULL == session) {
      throw odtunnel::string(_T("::WinHttpOpen() failed: ") + format_message(::GetLastError()));
    }

    connect = ::WinHttpConnect(session, widen(host).c_str(),
                               use_ssl ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
    if (NULL == connect) {
      throw odtunnel::string(_T("::WinHttpConnect() failed: ") + format_message(::GetLastError()));
    }

    request = ::WinHttpOpenRequest(connect, widen(method).c_str(), widen(path).c_str(), NULL, WINHTTP_NO_REFERER,
                                   WINHTTP_DEFAULT_ACCEPT_TYPES, use_ssl ? WINHTTP_FLAG_SECURE : 0);
    if (NULL == request) {
      throw odtunnel::string(_T("::WinHttpOpenRequest() failed: ") + format_message(::GetLastError()));
    }

    if (data_json.empty()) {
      if (!::WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        throw odtunnel::string(_T("::WinHttpSendRequest() failed: ") + format_message(::GetLastError()));
      }
    } else {
      DWORD content_size = static_cast<DWORD>(data_json.size());
      if (!::WinHttpSendRequest(request, L"Content-Type: application/json", static_cast<DWORD>(-1),
                                const_cast<char*>(data_json.data()), content_size, content_size, 0)) {
        throw odtunnel::string(_T("::WinHttpSendRequest() failed: ") + format_message(::GetLastError()));
      }
    }

    if (!::WinHttpReceiveResponse(request, NULL)) {
      throw odtunnel::string(_T("::WinHttpReceiveResponse() failed: ") + format_message(::GetLastError()));
    }

    while (true) {
      DWORD resp_size = 0;
      if (!::WinHttpQueryDataAvailable(request, &resp_size)) {
        throw odtunnel::string(_T("::WinHttpQueryDataAvailable() failed: ") + format_message(::GetLastError()));
      }
      if (0 == resp_size) {
        break;
      }

      std::string part(static_cast<size_t>(resp_size), '\0');
      if (!::WinHttpReadData(request, &(part[0]), resp_size, &resp_size)) {
        throw odtunnel::string(_T("::WinHttpReadData() failed: ") + format_message(::GetLastError()));
      }
      part.resize(resp_size);
      resp += part;
    }
  } catch(odtunnel::string e) {
    result.error_code_ = result::HTTPError;
    result.error_message_ = e;
  }

  // Close any open handles.
  if (request) ::WinHttpCloseHandle(request);
  if (connect) ::WinHttpCloseHandle(connect);
  if (session) ::WinHttpCloseHandle(session);

  if (result.is_ok()) {
    response = widen(resp);
  }

  return result;
}

} // namespace prv

::odtunnel::string system_region()
{
  TCHAR locale[LOCALE_NAME_MAX_LENGTH + 1];
  if (0 >= ::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, locale, sizeof(locale) / sizeof(TCHAR))) {
    return _T("US");
  }

  TCHAR* s = _tcschr(locale, _T('-'));
  if (nullptr == s) {
    return _T("US");
  }

  return ::odtunnel::string{s + 1};
}

} // namespace firebase
} // namespace odtunnel

