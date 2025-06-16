#include "trogondb/cmd/get_command.h"

namespace trogondb {
namespace cmd {

GetCommand::GetCommand(const std::shared_ptr<KeyValueStore> &store, const std::string &key)
    : m_key(key)
    , m_store(store)
{
    // ...
}

std::string GetCommand::execute()
{
    auto opt = m_store->getValue(m_key);
    if (opt) {
        std::string const &val = opt.value();
        return "$" + std::to_string(val.size()) + "\r\n" + val + "\r\n";
    }

    return "$-1\r\n";
}

} // namespace cmd
} // namespace trogondb
