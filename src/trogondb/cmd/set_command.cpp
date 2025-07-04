#include "trogondb/cmd/set_command.h"

#include <charconv>
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
        int64_t expiryMs = 0;
        auto result = std::from_chars(args[4].data(), args[4].data() + args[4].size(), expiryMs);
        if (result.ec != std::errc()) {
            return cmd::CommandResult::error(fmt::format("Invalid value of TTL '{}'", args[4]));
        }

        expiryTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(expiryMs);
    }

    m_store->setValue(args[0], args[1], expiryTime);

    return CommandResult::value("+OK\r\n");
}

} // namespace cmd
} // namespace trogondb
