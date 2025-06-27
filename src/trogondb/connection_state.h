#pragma once

#include <memory>

namespace trogondb {

class Connection;

class IConnectionState : public std::enable_shared_from_this<IConnectionState> {
public:
    IConnectionState(std::shared_ptr<Connection> connection);

    virtual void doRead(const std::string &data);

    virtual void doWrite();

    virtual void doTimeout();

    virtual ~IConnectionState() = default;

private:
    std::shared_ptr<Connection> m_connection;
};

class WaitingForArrayHeaderState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;

    void doRead(const std::string &data) override;
};

} // namespace trogondb
