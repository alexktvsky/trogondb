#include "trogondb/command_executor.h"

#include "trogondb/log/log_manager.h"
#include "trogondb/command.h"

namespace trogondb {

CommandExecutor::CommandExecutor(std::shared_ptr<KeyValueStore> store)
    : m_store(store)
    , m_commands()
{
    m_commands["ping"] = std::make_shared<PingCommand>();
    m_commands["echo"] = std::make_shared<EchoCommand>();
    m_commands["get"] = std::make_shared<GetCommand>(m_store);
    m_commands["set"] = std::make_shared<SetCommand>(m_store);
}

CommandResult CommandExecutor::execute(const std::string &commandName, const std::vector<std::string> &args)
{
    auto iter = m_commands.find(commandName);
    if (iter == m_commands.end()) {
        return CommandResult::error(fmt::format("unknown command '{}'\r\n", commandName));
    }

    std::shared_ptr<ICommand> cmd = iter->second;

    return cmd->execute(args);
}

} // namespace trogondb
