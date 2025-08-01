#pragma once

#include <memory>
#include <map>
#include <boost/asio.hpp>

#include "trogondb/proactor.h"
#include "trogondb/signal_handler.h"

namespace trogondb {

class SignalDispatcher {
public:
    SignalDispatcher(std::weak_ptr<Proactor> proactor);

    void addHandler(int signalNumber, std::shared_ptr<SignalHandler> handler);

    void start();

private:
    void wait();

    void handleSignal(const boost::system::error_code &err, int signalNumber);

    boost::asio::signal_set m_signalSet;
    std::map<int, std::shared_ptr<SignalHandler>> m_handlers;
};

} // namespace trogondb
