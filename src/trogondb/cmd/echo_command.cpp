#include "trogondb/cmd/echo_command.h"

namespace trogondb {
namespace cmd {

CommandResult EchoCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        return cmd::CommandResult::error("Invalid number of arguments"); // TODO
    }

    return CommandResult::value("$" + std::to_string(args[0].length()) + "\r\n" + args[0] + "\r\n");
}

} // namespace cmd
} // namespace trogondb
