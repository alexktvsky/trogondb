#include "trogondb/cmd/set_command.h"

#include <fmt/format.h>

#include "trogondb/utils.h"

namespace trogondb {
namespace cmd {

SetCommand::SetCommand(const std::shared_ptr<KeyValueStore> &store)
    : m_store(store)
{}

CommandResult SetCommand::execute(const std::vector<std::string> &args)
{
    if (args.size() != 2 && args.size() != 4) {
        return cmd::CommandResult::error("Invalid number of arguments"); // TODO
    }

    std::optional<std::chrono::steady_clock::time_point> expiryTime;

    if (args.size() == 4 && stringToLower(args[2]) == "px") {
        auto expiryMs = stringToNumber<int64_t>(args[3]);
        if (!expiryMs) {
            return cmd::CommandResult::error(fmt::format("Invalid value of TTL '{}'", args[3]));
        }

        expiryTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(expiryMs.value());
    }

    m_store->setValue(args[0], args[1], expiryTime);

    return CommandResult::value("+OK\r\n");
}

} // namespace cmd
} // namespace trogondb
