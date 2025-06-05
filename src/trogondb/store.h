#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <unordered_map>

#include "trogondb/entry.h"

namespace trogondb {

class Store {
public:
    void set(const std::string &key, const std::string &value,
             std::optional<std::chrono::steady_clock::time_point> expiry = std::nullopt);

    std::optional<std::string> get(const std::string &key);

    bool exists(const std::string &key) const;

    void del(const std::string &key);

private:
    std::unordered_map<std::string, Entry> m_data;
};

} // namespace trogondb
