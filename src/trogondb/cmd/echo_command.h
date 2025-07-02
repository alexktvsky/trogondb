#pragma once

#include <string>

#include "trogondb/cmd/command.h"

namespace trogondb {
namespace cmd {

class EchoCommand : public ICommand {
public:
    EchoCommand(const std::string &msg);
    CommandResult execute() override;
private:
    std::string m_msg;
};

} // namespace cmd
} // namespace trogondb
