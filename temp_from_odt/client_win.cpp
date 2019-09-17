#include "client_p.h"

#include <Windows.h>
#include <Rpc.h>

namespace odtunnel {
namespace client {
namespace prv {

::odtunnel::string _new_client_id()
{
  UUID uuid;
  ::UuidCreate(&uuid);
  RPC_CSTR uuid_str;
  ::UuidToStringA(&uuid, &uuid_str);
  std::string result(36, '\0');
  memcpy(&(result[0]), uuid_str, 36);
  ::RpcStringFreeA(&uuid_str);
  return widen(result);
}

} // namespace prv
} // namespace client
} // namespace odtunnel
