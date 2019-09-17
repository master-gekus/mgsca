#include "client_p.h"

#include <uuid/uuid.h>

namespace odtunnel {
namespace client {
namespace prv {

::odtunnel::string _new_client_id()
{
  uuid_t uuid;
  ::uuid_generate(uuid);
  char uuid_str[37];
  ::uuid_unparse(uuid, uuid_str);
  return uuid_str;
}

} // namespace prv
} // namespace client
} // namespace odtunnel
