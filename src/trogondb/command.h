#pragma once

#include <string>
#include <memory>
#include <vector>

#include "trogondb/command_result.h"
#include "trogondb/kv_store.h"

namespace trogondb {

class ICommand {
public:
    virtual CommandResult execute(const std::vector<std::string> &args) = 0;
    virtual ~ICommand() = default;
};

class PingCommand : public ICommand {
public:
    CommandResult execute(const std::vector<std::string> &args) override;
};

class EchoCommand : public ICommand {
public:
    CommandResult execute(const std::vector<std::string> &args) override;
};

class GetCommand : public ICommand {
public:
    GetCommand(const std::shared_ptr<KeyValueStore> &store);
    CommandResult execute(const std::vector<std::string> &args) override;

private:
    std::shared_ptr<KeyValueStore> m_store;
};

class SetCommand : public ICommand {
public:
    SetCommand(const std::shared_ptr<KeyValueStore> &store);
    CommandResult execute(const std::vector<std::string> &args) override;

private:
    std::shared_ptr<KeyValueStore> m_store;
};

} // namespace trogondb
