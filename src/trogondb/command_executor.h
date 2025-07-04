#pragma once

#include <memory>

#include "trogondb/cmd/command.h"
#include "trogondb/log/logger.h"
#include "trogondb/kv_store.h"

namespace trogondb {

class CommandExecutor {
public:
    CommandExecutor(std::shared_ptr<KeyValueStore> store);

    cmd::CommandResult execute(const std::string &commandName, const std::vector<std::string> &args);

private:
    std::shared_ptr<log::Logger> m_logger;
    std::shared_ptr<KeyValueStore> m_store;
    std::unordered_map<std::string, std::shared_ptr<cmd::ICommand>> m_commands;
};

} // namespace trogondb
