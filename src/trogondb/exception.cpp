#include "trogondb/exception.h"

namespace trogondb {

Exception::Exception(const std::string &message)
    : m_message(message)
{}

Exception::Exception(const Exception &other)
{
    m_message = other.m_message;
}

const char *Exception::what() const noexcept
{
    return m_message.c_str();
}

} // namespace trogondb
