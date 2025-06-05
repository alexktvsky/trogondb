#pragma once

#include <string>
#include <optional>
#include <chrono>

namespace trogondb {

class Entry {
public:
    Entry() = default;
    Entry(const std::string &value, const std::optional<std::chrono::steady_clock::time_point> &expiry = std::nullopt);
    Entry(const Entry &other);
    Entry &operator=(const Entry &other);
    Entry &operator=(Entry &&other) noexcept;
    Entry(Entry &&other) noexcept;
    const std::string &value() const;
    bool isExpired() const;

private:
    std::string m_value;
    std::optional<std::chrono::steady_clock::time_point> m_expiry;
};

} // namespace trogondb
