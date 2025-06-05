#pragma once

#include <string>

#include "trogondb/command/command.h"

namespace trogondb {

class PingCommand : public ICommand {
public:
    std::string execute() override;
};

inline std::string PingCommand::execute()
{
    return "+PONG\r\n";
}

} // namespace trogondb
