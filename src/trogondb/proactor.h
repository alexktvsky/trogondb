#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace trogondb {

class Acceptor;

class Proactor {
    friend class Acceptor;

public:
    Proactor();

    void run();

    void stop();

    bool isRunning() const;

private:
    std::shared_ptr<boost::asio::io_context> getImpl();

    std::shared_ptr<boost::asio::io_context> m_context;
};

} // namespace trogondb
