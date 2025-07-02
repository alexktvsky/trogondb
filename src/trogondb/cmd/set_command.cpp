#include "trogondb/cmd/set_command.h"

#include "trogondb/utils.h"

namespace trogondb {
namespace cmd {

SetCommand::SetCommand(const std::shared_ptr<KeyValueStore> &store, const std::vector<std::string> &args)
    : m_store(store)
{
    m_key = args[0];
    m_val = args[1];

    if (args.size() == 4 && stringToLower(args[2]) == "px") {
        m_expiryMs = std::stoll(args[4]);
    }
}

CommandResult SetCommand::execute()
{
    std::optional<std::chrono::steady_clock::time_point> expiryTime;
    if (m_expiryMs) {
        expiryTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(*m_expiryMs);
    }
    m_store->setValue(m_key, m_val, expiryTime);
    return CommandResult::value("+OK\r\n");
}

} // namespace cmd
} // namespace trogondb
