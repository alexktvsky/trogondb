#include "trogondb/signal_handler.h"

namespace trogondb {

SignalHandler::SignalHandler(std::weak_ptr<Server> server)
    : m_server(server)
{}

void SignalHandlerForShutdownRequest::handle()
{
    // TODO
}

void SignalHandlerForConfigReload::handle()
{
    // TODO
}

void SignalHandlerForCrashExit::handle()
{
    // TODO
}

} // namespace trogondb
