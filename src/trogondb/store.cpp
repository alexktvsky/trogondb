#include "trogondb/store.h"

namespace trogondb {

void Store::set(const std::string &key, const std::string &value,
                std::optional<std::chrono::steady_clock::time_point> expiry)
{
    m_data[key] = Entry(value, expiry);
}

std::optional<std::string> Store::get(const std::string &key)
{
    auto it = m_data.find(key);
    if (it == m_data.end()) {
        return std::nullopt;
    }

    if (it->second.isExpired()) {
        m_data.erase(it);
        return std::nullopt;
    }

    return it->second.value();
}

bool Store::exists(const std::string &key) const
{
    auto it = m_data.find(key);
    if (it == m_data.end()) return false;

    if (it->second.isExpired()) {
        // TODO: Add cleanup
        return false;
    }

    return true;
}

void Store::del(const std::string &key)
{
    m_data.erase(key);
}

} // namespace trogondb
