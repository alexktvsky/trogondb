#pragma once

extern "C" {
#include <signal.h>
}
#include <memory>

#include "trogondb/server.h"

namespace trogondb {

class SignalHandler {
public:
    SignalHandler(std::weak_ptr<Server> server);

    virtual void handle() = 0;

    virtual ~SignalHandler() = default;

private:
    std::weak_ptr<Server> m_server;
};

class SignalHandlerForShutdownRequest : public SignalHandler {
public:
    using SignalHandler::SignalHandler;

    void handle();
};

class SignalHandlerForConfigReload : public SignalHandler {
public:
    using SignalHandler::SignalHandler;

    void handle();
};

class SignalHandlerForCrashExit : public SignalHandler {
public:
    using SignalHandler::SignalHandler;

    void handle();
};

} // namespace trogondb
