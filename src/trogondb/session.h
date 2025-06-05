#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

#include "trogondb/command/command.h"
#include "trogondb/store.h"
#include "trogondb/logging/logger.h"
#include "trogondb/exception.h"

namespace trogondb {

enum class SessionState {
    WAITING_FOR_ARRAY_HEADER,
    WAITING_FOR_BULK_LENGTH,
    WAITING_FOR_BULK_BODY,
    READY_TO_EXECUTE,
    WRITING_RESPONSE,
    CLOSED,
    ERROR
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, const std::shared_ptr<Store> &store, const std::shared_ptr<logging::Logger> &logger);

    void start();

    void close();

private:
    void startTimeout();

    void onTimeout(const boost::system::error_code &err);

    void doReadLine();

    void onReadLine(const boost::system::error_code &err, size_t);

    void doReadBody();

    void onReadBody(const boost::system::error_code &err, size_t);

    std::unique_ptr<ICommand> createCommand(const std::vector<std::string> &args);

    void executeCommand();

    void doWrite();

    void onWrite(const boost::system::error_code &err, size_t n);

private:
    boost::asio::ip::tcp::socket m_socket;
    SessionState m_state;

    boost::asio::streambuf m_readBuffer;
    std::string m_writeBuffer;
    size_t m_writeOffset;

    int m_expectedArgsCount;
    int m_argsRead;
    int m_expectedBulkLength;
    std::vector<std::string> m_parsedArgs;

    static constexpr int TIMEOUT_SECONDS = 10;
    boost::asio::steady_timer m_timer;

    std::shared_ptr<Store> m_store;
    std::shared_ptr<logging::Logger> m_logger;
};

class UnknowCommandException : public Exception {
public:
    using Exception::Exception;
};

} // namespace trogondb
