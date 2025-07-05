#include "trogondb/command.h"

#include <optional>
#include <chrono>
#include <fmt/format.h>

#include "trogondb/utils.h"

namespace trogondb {

CommandResult PingCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 0) {
        return CommandResult::error("Invalid number of arguments"); // TODO
    }

    return CommandResult::value("+PONG\r\n");
}

CommandResult EchoCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        return CommandResult::error("Invalid number of arguments"); // TODO
    }

    return CommandResult::value("$" + std::to_string(args[0].length()) + "\r\n" + args[0] + "\r\n");
}

GetCommand::GetCommand(const std::shared_ptr<KeyValueStore> &store)
    : m_store(store)
{}

CommandResult GetCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        return CommandResult::error("Invalid number of arguments"); // TODO
    }

    auto opt = m_store->getValue(args[0]);
    if (!opt) {
        return CommandResult::value("$-1\r\n");
    }

    return CommandResult::value("$" + std::to_string(opt.value().size()) + "\r\n" + opt.value() + "\r\n");
}

SetCommand::SetCommand(const std::shared_ptr<KeyValueStore> &store)
    : m_store(store)
{}

CommandResult SetCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 2 && args.size() != 4) {
        return CommandResult::error("Invalid number of arguments"); // TODO
    }

    std::optional<std::chrono::steady_clock::time_point> expiryTime;

    if (args.size() == 4 && stringToLower(args[2]) == "px") {
        auto expiryMs = stringToNumber<int64_t>(args[3]);
        if (!expiryMs) {
            return CommandResult::error(fmt::format("Invalid value of TTL '{}'", args[3]));
        }

        expiryTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(expiryMs.value());
    }

    m_store->setValue(args[0], args[1], expiryTime);

    return CommandResult::value("+OK\r\n");
}


} // namespace trogondb
