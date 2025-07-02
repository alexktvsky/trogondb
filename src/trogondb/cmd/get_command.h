#pragma once

#include <string>
#include <memory>

#include "trogondb/cmd/command.h"
#include "trogondb/kv_store.h"

namespace trogondb {
namespace cmd {

class GetCommand : public ICommand {
public:
    GetCommand(const std::shared_ptr<KeyValueStore> &store, const std::string &key);
    CommandResult execute() override;

private:
    std::string m_key;
    std::shared_ptr<KeyValueStore> m_store;
};

} // namespace cmd
} // namespace trogondb
