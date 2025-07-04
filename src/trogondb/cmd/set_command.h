#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <memory>

#include "trogondb/cmd/command.h"
#include "trogondb/kv_store.h"

namespace trogondb {
namespace cmd {

class SetCommand : public ICommand {
public:
    SetCommand(const std::shared_ptr<KeyValueStore> &store);
    CommandResult execute(const std::vector<std::string> &args) override;

private:
    std::shared_ptr<KeyValueStore> m_store;
};

} // namespace cmd
} // namespace trogondb
