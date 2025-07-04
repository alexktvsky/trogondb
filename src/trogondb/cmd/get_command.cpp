#include "trogondb/cmd/get_command.h"

namespace trogondb {
namespace cmd {

GetCommand::GetCommand(const std::shared_ptr<KeyValueStore> &store)
    : m_store(store)
{}

CommandResult GetCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        return cmd::CommandResult::error("Invalid number of arguments"); // TODO
    }

    auto opt = m_store->getValue(args[0]);
    if (!opt) {
        return CommandResult::value("$-1\r\n");
    }

    return CommandResult::value("$" + std::to_string(opt.value().size()) + "\r\n" + opt.value() + "\r\n");
}

} // namespace cmd
} // namespace trogondb
