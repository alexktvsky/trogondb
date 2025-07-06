#pragma once

#include <memory>
#include <boost/asio.hpp>

#include "trogondb/connection.h"
#include "trogondb/log/logger.h"

namespace trogondb {

class Connection;

class IConnectionState : public std::enable_shared_from_this<IConnectionState> {
public:
    IConnectionState(std::weak_ptr<Connection> connection);

    virtual void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred);

    virtual void doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred);

    virtual void doTimeout();

    virtual ~IConnectionState() = default;

protected:
    std::shared_ptr<log::Logger> m_logger;
    std::weak_ptr<Connection> m_connection;
};

class ReadingHeaderState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;

    void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;
};

class ReadingArgumentLengthState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;

    void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;
};

class ReadingArgumentState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;

    void doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;
};

class ErrorState : public IConnectionState {
public:
    ErrorState(std::weak_ptr<Connection> connection, const std::string &output);

    void doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;

private:
    std::string m_output;
};

class WritingResponseState : public IConnectionState {
public:
    WritingResponseState(std::weak_ptr<Connection> connection, const std::string &output);

    void doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred) override;

private:
    std::string m_output;
};

class ClosedState : public IConnectionState {
public:
    using IConnectionState::IConnectionState;
};

} // namespace trogondb
