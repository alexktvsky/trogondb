#include "trogondb/signal_dispatcher.h"

namespace trogondb {

SignalDispatcher::SignalDispatcher(std::weak_ptr<Proactor> proactor)
    : m_signalSet(*proactor.lock()->getImpl())
{}

void SignalDispatcher::addHandler(int signalNumber, std::shared_ptr<SignalHandler> handler)
{
    m_handlers[signalNumber] = std::move(handler);
    m_signalSet.add(signalNumber);
}

void SignalDispatcher::start()
{
    wait();
}

void SignalDispatcher::wait()
{
    m_signalSet.async_wait(std::bind(&SignalDispatcher::handleSignal, this, std::placeholders::_1, std::placeholders::_2));
}

void SignalDispatcher::handleSignal(const boost::system::error_code &err, int signalNumber)
{
    if (err) {
        return;
    }

    auto it = m_handlers.find(signalNumber);
    if (it != m_handlers.end()) {
        it->second->handle();
    }

    wait(); // continue listening
}

} // namespace trogondb
