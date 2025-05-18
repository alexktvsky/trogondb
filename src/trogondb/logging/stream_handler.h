#pragma once

#include <cstdio>

#include "trogondb/logging/handler.h"
#include "trogondb/exception.h"

namespace trogondb {
namespace logging {

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

} // namespace logging
} // namespace trogondb
