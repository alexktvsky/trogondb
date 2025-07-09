#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <optional>
#include <charconv>
#include <string_view>

namespace trogondb {

std::string stringToLower(const std::string &s);

template <typename T>
std::optional<T> stringToNumber(const std::string_view &s)
{
    T value;

    if (std::from_chars(s.data(), s.data() + s.size(), value).ec != std::errc()) {
        return std::nullopt;
    }

    return value;
}

} // namespace trogondb
