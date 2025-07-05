#pragma once

#include <memory>

#include "trogondb/command.h"
#include "trogondb/kv_store.h"
#include "trogondb/log/logger.h"

namespace trogondb {

class CommandExecutor {
public:
    CommandExecutor(std::shared_ptr<KeyValueStore> store);

    CommandResult execute(const std::string &commandName, const std::vector<std::string> &args);

private:
    std::shared_ptr<KeyValueStore> m_store;
    std::unordered_map<std::string, std::shared_ptr<ICommand>> m_commands;
};

} // namespace trogondb
