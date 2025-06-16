#include "trogondb/cmd/ping_command.h"

namespace trogondb {
namespace cmd {

std::string PingCommand::execute()
{
    return "+PONG\r\n";
}

} // namespace cmd
} // namespace trogondb
