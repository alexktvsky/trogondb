#pragma once

#include <memory>
#include <boost/asio.hpp>

#include "trogondb/connection.h"
#include "trogondb/log/logger.h"

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
    std::shared_ptr<log::Logger> m_logger;
};


class ReadingHeaderState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;

    void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;
};


class ReadingArgumentLengthState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;
};


class ErrorState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;
};

} // namespace trogondb
