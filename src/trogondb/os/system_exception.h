#pragma once

#include "trogondb/exception.h"

namespace trogondb {
namespace os {

class SystemException : public Exception {
public:
    using Exception::Exception;
};

} // namespace os
} // namespace trogondb
