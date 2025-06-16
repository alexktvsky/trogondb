#include "trogondb/cmd/echo_command.h"

namespace trogondb {
namespace cmd {

EchoCommand::EchoCommand(const std::string &msg)
    : m_msg(msg)
{
    // ...
}

std::string EchoCommand::execute()
{
    return "$" + std::to_string(m_msg.size()) + "\r\n" + m_msg + "\r\n";
}

} // namespace cmd
} // namespace trogondb
