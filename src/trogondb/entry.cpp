#include "trogondb/entry.h"

#include <utility>

namespace trogondb {

Entry::Entry(std::string val, std::optional<std::chrono::steady_clock::time_point> exp)
    : m_value(std::move(val))
    , m_expiry(std::move(exp))
{
    // ...
}

const std::string& Entry::value() const
{
    return m_value;
}

bool Entry::isExpired() const
{
    return m_expiry && std::chrono::steady_clock::now() >= *m_expiry;
}

} // namespace trogondb

