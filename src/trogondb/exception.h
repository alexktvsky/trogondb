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

inline Exception::Exception(const std::string &message)
    : m_message(message)
{}

inline Exception::Exception(const Exception &other)
{
    m_message = other.m_message;
}

inline const char *Exception::what() const noexcept
{
    return m_message.c_str();
}

} // namespace trogondb
