#include "config_parser.h"

#include <algorithm>
#include <memory>

namespace {

class SizeWithUnit {
public:
    SizeWithUnit();

    SizeWithUnit(size_t value);

    SizeWithUnit(const std::string &str);

    operator size_t() const { return m_value; }

private:
    static size_t parseString(const std::string &str);
    static bool isNumericString(const std::string &str);

private:
    size_t m_value;
};

SizeWithUnit::SizeWithUnit()
    : SizeWithUnit(0) {}

SizeWithUnit::SizeWithUnit(size_t value)
    : m_value(value) {}

SizeWithUnit::SizeWithUnit(const std::string &str)
    : m_value(parseString(str)) {}

bool SizeWithUnit::isNumericString(const std::string &str)
{
    for (const auto &c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }

    return true;
}

size_t SizeWithUnit::parseString(const std::string &str)
{
    if (str.empty()) {
        throw std::invalid_argument("Empty size string");
    }

    size_t multiplier = 1;
    std::string sizeStr = str;
    char lastChar = str.back();

    if (!std::isdigit(lastChar)) {
        sizeStr = str.substr(0, str.size() - 1);

        char unit = lastChar;
        switch (unit) {
        case 'K':
        case 'k':
            multiplier = 1024;
            break;
        case 'M':
        case 'm':
            multiplier = 1024 * 1024;
            break;
        case 'G':
        case 'g':
            multiplier = 1024 * 1024 * 1024;
            break;
        default:
            throw std::invalid_argument("Unknown size unit");
        }
    }

    if (!isNumericString(sizeStr)) {
        throw std::invalid_argument("Invalid size");
    }

    size_t size = std::stoul(sizeStr);

    return size * multiplier;
}

} //namespace

namespace YAML {

template <>
struct convert<SizeWithUnit> {
    static Node encode(SizeWithUnit rhs)
    {
        return Node(std::to_string(static_cast<size_t>(rhs)));
    }

    static bool decode(const Node &node, SizeWithUnit &rhs)
    {
        if (!node.IsScalar()) {
            return false;
        }
        try {
            rhs = SizeWithUnit(node.Scalar());
        }
        catch (...) {
            return false;
        }

        return true;
    }
};

} // namespace YAML

namespace trogondb {

std::shared_ptr<ServerConfig> ConfigParser::parseFile(const std::string &fileName)
{
    auto config = std::make_shared<ServerConfig>();

    Node rootNode = Node::createRootNode(fileName);

    config->host = rootNode.getValue<std::string>("host");
    config->port = rootNode.getValue<uint16_t>("port");
    config->daemon = rootNode.getValue<bool>("daemon", false);
    config->workdir = rootNode.getValue<std::string>("workdir", "");
    config->priority = rootNode.getValue<int>("priority", 0);
    config->user = rootNode.getValue<std::string>("user", "");
    config->group = rootNode.getValue<std::string>("group", "");

    Node logNode = rootNode.getChild("log");
    for (size_t i = 0; i < logNode.size(); ++i) {
        std::string target = logNode[i].getValue<std::string>("target");
        std::string level = logNode[i].getValue<std::string>("level");
        size_t limit = logNode[i].getValue<SizeWithUnit>("limit", 0);
        unsigned int rotate = logNode[i].getValue<unsigned int>("rotate", 1);
        config->logs.push_back({target, level, limit, rotate});
    }

    rootNode.checkForUnusedNodes();

    return config;
}

} // namespace trogondb
