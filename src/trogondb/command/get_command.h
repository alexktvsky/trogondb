#pragma once

#include <string>
#include <memory>

#include "trogondb/command/command.h"
#include "trogondb/kv_store.h"

namespace trogondb {

class GetCommand : public ICommand {
public:
    GetCommand(const std::shared_ptr<KeyValueStore> &store, const std::string &key);
    std::string execute() override;

private:
    std::string m_key;
    std::shared_ptr<KeyValueStore> m_store;
};

inline GetCommand::GetCommand(const std::shared_ptr<KeyValueStore> &store, const std::string &key)
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

} // namespace trogondb
