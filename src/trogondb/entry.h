#pragma once

#include <string>
#include <optional>
#include <chrono>

namespace trogondb {

class Entry {
public:
    Entry() = default;
    Entry(std::string val, std::optional<std::chrono::steady_clock::time_point> exp = std::nullopt);
    const std::string& value() const;
    bool isExpired() const;

private:
    std::string m_value;
    std::optional<std::chrono::steady_clock::time_point> m_expiry;
};

} // namespace trogondb
