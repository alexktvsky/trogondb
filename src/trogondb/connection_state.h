#pragma once

#include <memory>

namespace trogondb {

class IConnectionState : public std::enable_shared_from_this<IConnectionState> {
public:
    virtual std::shared_ptr<IConnectionState> handle();
    virtual ~IConnectionState() = default;
private:

};

} // namespace trogondb
