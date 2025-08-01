#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace trogondb {

class Acceptor;

class Proactor {

public:
    Proactor();

    void run();

    void stop();

    bool isRunning() const;

    std::shared_ptr<boost::asio::io_context> getImpl();

private:
    std::shared_ptr<boost::asio::io_context> m_context;
};

} // namespace trogondb
