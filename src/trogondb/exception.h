#pragma once

#include <exception>
#include <string>

namespace trogondb {

class Exception : public std::exception {
public:
    explicit Exception(const std::string &message);
    explicit Exception(const Exception &other);
    virtual const char *what() const noexcept override;
private:
    std::string m_message;
};

} // namespace trogondb
