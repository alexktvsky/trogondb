#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <boost/asio.hpp>

#include "trogondb/cmd/command.h"
#include "trogondb/kv_store.h"
#include "trogondb/log/logger.h"
#include "trogondb/exception.h"

namespace trogondb {

enum class ConnectionState : uint8_t {
    WAITING_FOR_ARRAY_HEADER,
    WAITING_FOR_BULK_LENGTH,
    WAITING_FOR_BULK_BODY,
    READY_TO_EXECUTE,
    WRITING_RESPONSE,
    CLOSED,
    ERROR
};

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(boost::asio::ip::tcp::socket socket, std::shared_ptr<log::Logger> logger);

    void start();

    void cancel();

private:
//     void startTimeout();

//     void onTimeout(const boost::system::error_code &err);

//     void doReadLine();

//     void onReadLine(const boost::system::error_code &err, size_t /*unused*/);

//     void doReadBody();

//     void onReadBody(const boost::system::error_code &err, size_t /*unused*/);

//     std::optional<std::unique_ptr<cmd::ICommand>> createCommand(const std::string &cmd, const std::vector<std::string> &args);

//     void executeCommand();

//     void doWrite();

//     void onWrite(const boost::system::error_code &err, size_t n);

private:
    boost::asio::ip::tcp::socket m_socket;
    std::shared_ptr<log::Logger> m_logger;

//     ConnectionState m_state;

//     boost::asio::streambuf m_readBuffer;
//     std::string m_writeBuffer;
//     size_t m_writeOffset;

//     int m_expectedArgsCount;
//     int m_argsRead;
//     int m_expectedBulkLength;
//     std::vector<std::string> m_parsedArgs;

//     static constexpr int TIMEOUT_SECONDS = 10;
//     boost::asio::steady_timer m_timer;

//     std::shared_ptr<KeyValueStore> m_store;
//     std::shared_ptr<log::Logger> m_logger;

//     bool m_cancelled;
};



} // namespace trogondb
