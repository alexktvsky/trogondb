#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

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
    Session(boost::asio::ip::tcp::socket socket, Server &server);

    void start();

private:
    void startTimeout();

    void onTimeout(const boost::system::error_code &err);

    void doReadLine();

    void onReadLine(const boost::system::error_code &err, size_t);

    void doReadBody();

    void onReadBody(const boost::system::error_code &err, size_t);

    std::unique_ptr<Command> makeCommand(const std::vector<std::string> &args);

    void executeCommand();

    void doWrite();

    void onWrite(const boost::system::error_code &err, size_t n);

    void closeWithError(const boost::system::error_code &err, const char *where);

    void enterError(const char *msg);

    static std::string toLower(const std::string& s);

private:
    boost::asio::ip::tcp::socket m_socket;
    Server &m_server;
    SessionState m_state;

    boost::asio::streambuf m_readBuffer;
    std::string m_writeBuffer;
    size_t m_writeOffset;

    int m_expectedArgsCount;
    int m_argsRead;
    int m_expectedBulkLength;
    std::vector<std::string> m_parsedArgs;

    boost::asio::steady_timer m_timer;
    static constexpr int TIMEOUT_SECONDS = 10;
};

} // namespace trogondb
