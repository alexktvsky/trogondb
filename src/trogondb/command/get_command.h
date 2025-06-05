#pragma once

#include <string>
#include <memory>

#include "trogondb/command/command.h"
#include "trogondb/store.h"

namespace trogondb {

class GetCommand : public ICommand {
public:
    GetCommand(const std::shared_ptr<Store> &store, const std::string &key);
    std::string execute() override;

private:
    std::shared_ptr<Store> m_store;
    std::string m_key;
};

inline GetCommand::GetCommand(const std::shared_ptr<Store> &store, const std::string &key)
    : m_key(key)
{
    // ...
}

std::string GetCommand::execute()
{
    auto opt = m_store->getValue(m_key);
    if (opt) {
        std::string val = opt.value();
        return "$" + std::to_string(val.size()) + "\r\n" + val + "\r\n";
    }
    return "$-1\r\n";
}

} // namespace trogondb
