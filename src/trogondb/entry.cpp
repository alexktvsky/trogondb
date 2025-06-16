#include "trogondb/entry.h"

namespace trogondb {

Entry::Entry(const std::string &value, const std::optional<std::chrono::steady_clock::time_point> &expiry)
    : m_value(value)
    , m_expiry(expiry)
{}

Entry::Entry(const Entry &other)
    : m_value(other.m_value)
    , m_expiry(other.m_expiry)
{}

Entry::Entry(Entry &&other) noexcept
    : m_value(std::move(other.m_value))
    , m_expiry(std::move(other.m_expiry))
{}

Entry &Entry::operator=(const Entry &other)
{
    if (this == &other) {
        return *this;
    }

    m_value = other.m_value;
    m_expiry = other.m_expiry;

    return *this;
}

Entry &Entry::operator=(Entry &&other) noexcept
{
    if (this == &other) {
        return *this;
    }

    m_value = std::move(other.m_value);
    m_expiry = std::move(other.m_expiry);

    return *this;
}

const std::string &Entry::value() const
{
    return m_value;
}

bool Entry::isExpired() const
{
    return m_expiry && std::chrono::steady_clock::now() >= *m_expiry;
}

} // namespace trogondb
