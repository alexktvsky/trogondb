#include "trogondb/cmd/ping_command.h"

namespace trogondb {
namespace cmd {

CommandResult PingCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 0) {
        return cmd::CommandResult::error("Invalid number of arguments"); // TODO
    }

    return CommandResult::value("+PONG\r\n");
}

} // namespace cmd
} // namespace trogondb
