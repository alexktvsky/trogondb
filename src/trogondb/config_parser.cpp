#include "config_parser.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

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

namespace {

class Node {
public:
    static Node createRootNode(const std::string &filename);
    Node getChild(const std::string &nodeName) const;
    template <typename T>
    T getValue(const std::string &keyName) const;
    template <typename T>
    T getValue(const std::string &keyName, const T &defaultValue) const;
    template <typename T>
    T getValue(const std::string &key, const std::vector<T> &possibleValues) const;
    template <typename T>
    T getValue(const std::string &key, const T &defaultValue, const std::vector<T> &possibleValues) const;
    Node operator[](size_t index) const;
    size_t size() const;
    bool operator==(const Node &other);
    std::string getFullPath() const;
    std::string getFilename() const;
    unsigned int getLineInFile() const;
    unsigned int getColumnInFile() const;
    int getRealColumnInFile() const;
    std::list<Node> getUsedNodes() const;
    std::list<Node> getNodes() const;
private:
    Node(const YAML::Node &node,
         const std::optional<YAML::Node> &parent,
         const std::string &path,
         const std::string &filename,
         const std::shared_ptr<std::stringstream> &fileBuffer,
         const std::shared_ptr<std::list<Node>> &usedNodes);
    void traverseNode(const Node &node, std::list<Node> *nodes) const;
private:
    YAML::Node m_node;
    std::optional<YAML::Node> m_parent;
    std::string m_path;
    std::string m_fileName;
    std::shared_ptr<std::stringstream> m_fileBuffer;
    mutable std::shared_ptr<std::list<Node>> m_usedNodes;
};

Node::Node(const YAML::Node &node,
           const std::optional<YAML::Node> &parent,
           const std::string &path,
           const std::string &filename,
           const std::shared_ptr<std::stringstream> &fileBuffer,
           const std::shared_ptr<std::list<Node>> &usedNodes)
    : m_node(node)
    , m_parent(parent)
    , m_path(path)
    , m_fileName(filename)
    , m_fileBuffer(fileBuffer)
    , m_usedNodes(usedNodes)
{
    // ...
}

Node Node::createRootNode(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw trogondb::ConfigFileException(
            fmt::format("Failed to open config file '{}'", filename));
    }

    auto fileBuffer = std::make_shared<std::stringstream>();
    *fileBuffer << file.rdbuf();
    file.close();

    YAML::Node node;
    try {
        node = YAML::Load(fileBuffer->str());
    }
    catch (const YAML::BadFile &e) {
        throw trogondb::ConfigFileException(
            fmt::format("Failed to load config file '{}'", filename));
    }

    auto usedNodes = std::make_shared<std::list<Node>>();

    return Node(node, std::nullopt, "", filename, fileBuffer, usedNodes);
}

Node Node::getChild(const std::string &key) const
{
    std::string childPath = m_path.empty() ? key : fmt::format("{}.{}", m_path, key);

    YAML::Node childNode = m_node[key];

    if (!childNode) {
        throw trogondb::ConfigFileException(
            fmt::format("YAML parse error on {}: Missing keyword '{}'", m_fileName, childPath));
    }

    Node createdNode = Node(childNode, m_node, childPath, m_fileName, m_fileBuffer, m_usedNodes);

    m_usedNodes->push_back(createdNode);

    return createdNode;
}

template <typename T>
T Node::getValue(const std::string &key) const
{
    std::string childPath = m_path.empty() ? key : fmt::format("{}.{}", m_path, key);

    YAML::Node childNode = m_node[key];

    if (!childNode) {
        throw trogondb::ConfigFileException(
            fmt::format("YAML parse error on {}: Missing keyword '{}'", m_fileName, childPath));
    }

    try {
        m_usedNodes->push_back(Node(childNode, m_node, childPath, m_fileName, m_fileBuffer, m_usedNodes));
        return childNode.as<T>();
    }
    catch (const YAML::TypedBadConversion<T> &e) {
        int lineNumber = e.mark.line + 1;
        int columnNumber = e.mark.column + 1;
        std::string valueAsString = childNode.as<std::string>();
        throw trogondb::ConfigFileException(
            fmt::format("YAML parse error on  {}:{}:{}: Invalid value '{}' for keyword '{}'",
                        m_fileName,
                        lineNumber,
                        columnNumber,
                        valueAsString,
                        childPath));
    }
}

template <typename T>
T Node::getValue(const std::string &key, const T &defaultValue) const
{
    std::string childPath = m_path.empty() ? key : fmt::format("{}.{}", m_path, key);

    YAML::Node childNode = m_node[key];

    if (!childNode) {
        return defaultValue;
    }

    try {
        m_usedNodes->push_back(Node(childNode, m_node, childPath, m_fileName, m_fileBuffer, m_usedNodes));
        return childNode.as<T>();
    }
    catch (const YAML::TypedBadConversion<T> &e) {
        int line = e.mark.line + 1;
        int column = e.mark.column + 1;
        std::string valueAsString = childNode.as<std::string>();
        throw trogondb::ConfigFileException(
            fmt::format("YAML parse error on {}:{}:{}: Invalid value '{}' for keyword '{}'",
                        m_fileName,
                        line,
                        column,
                        valueAsString,
                        childPath));
    }
}

template <typename T>
T Node::getValue(const std::string &key, const std::vector<T> &possibleValues) const
{
    T value = this->getValue<T>(key);
    auto iter = std::find(possibleValues.begin(), possibleValues.end(), value);
    if (iter == possibleValues.end()) {
        int lineNumber = this->getLineInFile();
        int columnNumber = this->getColumnInFile();
        std::string valueAsString = m_node[key].as<std::string>();
        std::string path = this->getFullPath();
        throw trogondb::ConfigFileException(
            fmt::format("YAML parse error on {}:{}:{}: Invalid value '{}' for keyword '{}'",
                        m_fileName,
                        lineNumber,
                        columnNumber,
                        valueAsString,
                        path));
    }
    return value;
}

template <typename T>
T Node::getValue(const std::string &key, const T &defaultValue, const std::vector<T> &possibleValues) const
{
    T value = this->getValue<T>(key, defaultValue);
    auto iter = std::find(possibleValues.begin(), possibleValues.end(), value);
    if (iter == possibleValues.end()) {
        int lineNumber = this->getLineInFile();
        int columnNumber = this->getColumnInFile();
        std::string valueAsString = m_node[key].as<std::string>();
        std::string path = this->getFullPath();
        throw trogondb::ConfigFileException(
            fmt::format("YAML parse error on {}:{}:{}: Invalid value '{}' for keyword '{}'",
                        m_fileName,
                        lineNumber,
                        columnNumber,
                        valueAsString,
                        path));
    }
    return value;
}

Node Node::operator[](size_t index) const
{
    std::string childPath = fmt::format("{}[{}]", m_path, index);

    Node createdNode = Node(m_node[index], m_node, childPath, m_fileName, m_fileBuffer, m_usedNodes);

    m_usedNodes->push_back(createdNode);

    return createdNode;
}

size_t Node::size() const
{
    return m_node.size();
}

bool Node::operator==(const Node &other)
{
    return this->m_node == other.m_node;
}

std::string Node::getFullPath() const
{
    return m_path;
}

std::string Node::getFilename() const
{
    return m_fileName;
}

unsigned int Node::getLineInFile() const
{
    return m_node.Mark().line + 1;
}

unsigned int Node::getColumnInFile() const
{
    return m_node.Mark().column + 1;
}

std::list<Node> Node::getUsedNodes() const
{
    return std::list<Node>(*m_usedNodes);
}

std::list<Node> Node::getNodes() const
{
    std::list<Node> nodes;

    // Find all nodes starting from the current node
    traverseNode(*this, &nodes);

    return nodes;
}

void Node::traverseNode(const Node &node, std::list<Node> *nodes) const
{
    if (node.m_node.IsMap()) {
        for (YAML::const_iterator it = node.m_node.begin(); it != node.m_node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            std::string childPath = node.m_path.empty() ? key : fmt::format("{}.{}", node.m_path, key);
            traverseNode(Node(it->second, node.m_node, childPath, node.m_fileName, node.m_fileBuffer, node.m_usedNodes), nodes);
        }
    }
    else if (node.m_node.IsSequence()) {
        for (size_t i = 0; i < node.m_node.size(); ++i) {
            std::string childPath = fmt::format("{}[{}]", node.m_path, i);
            traverseNode(Node(node.m_node[i], node.m_node, childPath, node.m_fileName, node.m_fileBuffer, node.m_usedNodes), nodes);
        }
    }

    if (!node.m_path.empty()) {
        nodes->push_back(node);
    }
}

std::string getLastElementInNodePath(const std::string &path)
{
    size_t lastDotPos = path.find_last_of('.');

    if (lastDotPos == std::string::npos) {
        return path;
    }

    return path.substr(lastDotPos + 1);
}

int Node::getRealColumnInFile() const
{
    std::string key = getLastElementInNodePath(getFullPath());
    int lineNumber = getLineInFile() - 1;
    int columnNumber = getColumnInFile() - 1;

    std::string line;
    int currentLine = 0;

    while (std::getline(*m_fileBuffer, line)) {
        if (currentLine == lineNumber) {
            // Check the bounds for column
            if (columnNumber < 0 || columnNumber >= line.length()) {
                return -1;
            }

            // Search for the string starting from the specified column to the left
            for (int i = columnNumber; i >= 0; --i) {
                if (line.substr(i, key.length()) == key) {
                    return i + 1;
                }
            }
            return -1; // String not found in the specified range
        }
        ++currentLine;
    }

    return -1; // Line number out of range
}

void checkForUnusedNodes(const Node &rootNode)
{
    std::list<Node> usedNodes = rootNode.getUsedNodes();
    std::list<Node> nodes = rootNode.getNodes();
    std::string message;
    bool checkFailed = false;

    for (const auto &node : nodes) {
        auto iter = std::find(usedNodes.begin(), usedNodes.end(), node);
        if (iter == usedNodes.end()) {
            checkFailed = true;
            if (!message.empty()) {
                message += "\n";
            }

            int columnNumber = node.getRealColumnInFile();
            if (columnNumber == -1) {
                columnNumber = node.getColumnInFile();
            }
            message += fmt::format("YAML parse error on {}:{}:{}: Unknown keyword '{}'",
                                   node.getFilename(),
                                   node.getLineInFile(),
                                   columnNumber,
                                   node.getFullPath());
        }
    }

    if (checkFailed) {
        throw trogondb::ConfigFileException(message);
    }
}

} //namespace

namespace trogondb {

std::shared_ptr<ServerConfig> ConfigParser::parseFile(const std::string &filename)
{
    auto config = std::make_shared<ServerConfig>();

    Node rootNode = Node::createRootNode(filename);

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

    checkForUnusedNodes(rootNode);

    return config;
}

} // namespace trogondb
