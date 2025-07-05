#include "trogondb/command_executor.h"

#include "trogondb/log/log_manager.h"
#include "trogondb/cmd/ping_command.h"
#include "trogondb/cmd/echo_command.h"
#include "trogondb/cmd/get_command.h"
#include "trogondb/cmd/set_command.h"

namespace trogondb {

CommandExecutor::CommandExecutor(std::shared_ptr<KeyValueStore> store)
    : m_store(store)
    , m_commands()
{
    m_commands["ping"] = std::make_shared<cmd::PingCommand>();
    m_commands["echo"] = std::make_shared<cmd::EchoCommand>();
    m_commands["get"] = std::make_shared<cmd::GetCommand>(m_store);
    m_commands["set"] = std::make_shared<cmd::SetCommand>(m_store);
}

cmd::CommandResult CommandExecutor::execute(const std::string &commandName, const std::vector<std::string> &args)
{
    auto iter = m_commands.find(commandName);
    if (iter == m_commands.end()) {
        return cmd::CommandResult::error(fmt::format("unknown command '{}'\r\n", commandName));
    }

    std::shared_ptr<cmd::ICommand> cmd = iter->second;

    return cmd->execute(args);
}

} // namespace trogondb
