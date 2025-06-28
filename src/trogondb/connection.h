#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <boost/asio.hpp>

#include "trogondb/log/logger.h"
#include "trogondb/connection_state.h"

namespace trogondb {

class IConnectionState;

class Connection : public std::enable_shared_from_this<Connection> {
    friend class IConnectionState;
    friend class ReadingHeaderState;
    friend class ReadingArgumentLengthState;
    friend class ReadingArgumentState;

public:
    Connection(boost::asio::ip::tcp::socket socket);

    void start();

    void cancel();

private:
    void changeState(std::shared_ptr<IConnectionState> state);

    void doRead();

    void onReadDone(const boost::system::error_code &err, size_t bytesTransferred);

    void doWrite();

    void onWriteDone(const boost::system::error_code &err, size_t bytesTransferred);


    // TODO: move to another class
    void executeCommand(const std::string &cmd, const std::vector<std::string> &args);


    std::shared_ptr<log::Logger> m_logger;
    std::shared_ptr<IConnectionState> m_state;

    boost::asio::ip::tcp::socket m_socket;
    std::shared_ptr<boost::asio::streambuf> m_readBuffer;
    std::shared_ptr<boost::asio::streambuf> m_writeBuffer;
    // boost::asio::steady_timer m_timer;
    std::atomic<bool> m_cancelled;

    struct Context {
        uint32_t expectedArgsCount;
        size_t expectedNextBulkLength;

        std::string cmd;
        std::vector<std::string> args;
    };

    Context m_context;
};

} // namespace trogondb
