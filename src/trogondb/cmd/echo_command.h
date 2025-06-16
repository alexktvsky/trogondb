#pragma once

#include <string>

#include "trogondb/command.h"

namespace trogondb {

class EchoCommand : public ICommand {
public:
    EchoCommand(const std::string &msg);
    std::string execute() override;
private:
    std::string m_msg;
};

inline EchoCommand::EchoCommand(const std::string &msg)
    : m_msg(msg)
{
    // ...
}

inline std::string EchoCommand::execute()
{
    return "$" + std::to_string(m_msg.size()) + "\r\n" + m_msg + "\r\n";
}

} // namespace trogondb
