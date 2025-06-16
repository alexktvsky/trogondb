#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <memory>

#include "trogondb/command.h"
#include "trogondb/kv_store.h"
#include "trogondb/utils.h"

namespace trogondb {

class SetCommand : public ICommand {
public:
    SetCommand(const std::shared_ptr<KeyValueStore> &store, const std::vector<std::string> &args);
    std::string execute() override;

private:
    std::string m_key;
    std::string m_val;
    std::optional<int64_t> m_expiryMs;
    std::shared_ptr<KeyValueStore> m_store;
};

inline SetCommand::SetCommand(const std::shared_ptr<KeyValueStore> &store, const std::vector<std::string> &args)
    : m_store(store)
{
    m_key = args[1];
    m_val = args[2];

    if (args.size() == 5 && stringToLower(args[3]) == "px") {
        m_expiryMs = std::stoll(args[4]);
    }
}

inline std::string SetCommand::execute()
{
    std::optional<std::chrono::steady_clock::time_point> expiryTime;
    if (m_expiryMs) {
        expiryTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(*m_expiryMs);
    }
    m_store->setValue(m_key, m_val, expiryTime);
    return "+OK\r\n";
}

} // namespace trogondb
