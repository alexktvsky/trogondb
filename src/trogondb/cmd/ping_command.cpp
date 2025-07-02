#include "trogondb/cmd/ping_command.h"

namespace trogondb {
namespace cmd {

CommandResult PingCommand::execute()
{
    return CommandResult::value("+PONG\r\n");
}

} // namespace cmd
} // namespace trogondb
