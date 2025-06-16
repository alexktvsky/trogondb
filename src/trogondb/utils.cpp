#include "trogondb/utils.h"

namespace trogondb {

std::string stringToLower(const std::string &s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace trogondb
