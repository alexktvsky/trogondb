#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace trogondb {

class Connection;

class IConnectionState : public std::enable_shared_from_this<IConnectionState> {
public:
    IConnectionState(std::shared_ptr<Connection> connection);

    virtual void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred);

    virtual void doWrite();

    virtual void doTimeout();

    virtual ~IConnectionState() = default;

protected:
    std::shared_ptr<Connection> m_connection;
};

class ReadingHeaderState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;

    void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;
};

} // namespace trogondb
