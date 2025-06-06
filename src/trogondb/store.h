#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <unordered_map>

#include "trogondb/entry.h"

namespace trogondb {

class Store {
public:
    Store() = default;

    void setValue(const std::string &key, const std::string &value, std::optional<std::chrono::steady_clock::time_point> expiry = std::nullopt);

    std::optional<std::string> getValue(const std::string &key);

    bool isExists(const std::string &key);

    void delValue(const std::string &key);

private:
    std::unordered_map<std::string, Entry> m_data;
};

} // namespace trogondb
