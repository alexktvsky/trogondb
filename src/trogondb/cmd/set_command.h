#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <memory>

#include "trogondb/cmd/command.h"
#include "trogondb/kv_store.h"
#include "trogondb/utils.h"

namespace trogondb {
namespace cmd {

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

} // namespace cmd
} // namespace trogondb
