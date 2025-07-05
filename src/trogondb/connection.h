#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <boost/asio.hpp>

#include "trogondb/log/logger.h"
#include "trogondb/connection_state.h"
#include "trogondb/connection_manager.h"

namespace trogondb {

class IConnectionState;
class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection> {
    friend class IConnectionState;
    friend class ReadingHeaderState;
    friend class ReadingArgumentLengthState;
    friend class ReadingArgumentState;
    friend class WritingResponseState;
    friend class ClosedState;
    friend class ErrorState;

public:
    Connection(std::shared_ptr<ConnectionManager> connectionManager, boost::asio::ip::tcp::socket socket);

    void start();

    void close();

    bool isClosed() const;

private:
    void changeState(std::shared_ptr<IConnectionState> state);

    void doRead();

    void onReadDone(const boost::system::error_code &err, size_t bytesTransferred);

    void doWrite();

    void onWriteDone(const boost::system::error_code &err, size_t bytesTransferred);

    std::shared_ptr<ConnectionManager> m_connectionManager;
    std::shared_ptr<log::Logger> m_logger;
    std::shared_ptr<IConnectionState> m_state;
    boost::asio::ip::tcp::socket m_socket;
    std::shared_ptr<boost::asio::streambuf> m_readBuffer;
    std::shared_ptr<boost::asio::streambuf> m_writeBuffer;
    // boost::asio::steady_timer m_timer;
    std::atomic<bool> m_cancelled;
    std::mutex m_closeMutex;

    struct ParseContext {
        uint32_t expectedArgsCount;
        size_t expectedNextBulkLength;
        std::string cmd;
        std::vector<std::string> args;
    };
    ParseContext m_context;
};

} // namespace trogondb
