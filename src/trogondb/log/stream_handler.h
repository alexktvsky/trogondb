#pragma once

#include <cstdio>

#include "trogondb/log/handler.h"
#include "trogondb/exception.h"

namespace trogondb {
namespace log {

class StreamHandler : public Handler {
public:
    StreamHandler(std::FILE *stream);
private:
    static std::shared_ptr<HandlerImpl> createStreamHandler(std::FILE *stream);
};

class InvalidStreamException : public Exception {
public:
    using Exception::Exception;
};

} // namespace log
} // namespace trogondb
