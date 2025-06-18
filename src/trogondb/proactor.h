#pragma once

#include <boost/asio.hpp>

namespace trogondb {

class Proactor {
public:
    void run();
    void stop();
    bool isRunning();
private:
    boost::asio::io_context m_executionContext;
};

} // namespace trogondb
