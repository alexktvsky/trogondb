#include "config_node.h"

#include <fstream>

namespace trogondb {

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
        throw ConfigFileException(fmt::format("Failed to open config file '{}'", filename));
    }

    auto fileBuffer = std::make_shared<std::stringstream>();
    *fileBuffer << file.rdbuf();
    file.close();

    YAML::Node node;
    try {
        node = YAML::Load(fileBuffer->str());
    }
    catch (const YAML::BadFile &e) {
        throw ConfigFileException(fmt::format("Failed to load config file '{}'", filename));
    }

    auto usedNodes = std::make_shared<std::list<Node>>();

    return Node(node, std::nullopt, "", filename, fileBuffer, usedNodes);
}

Node Node::getChild(const std::string &key) const
{
    std::string childPath = m_path.empty() ? key : fmt::format("{}.{}", m_path, key);

    YAML::Node childNode = m_node[key];

    if (!childNode) {
        throw ConfigFileException(fmt::format("YAML parse error on {}: Missing keyword '{}'", m_fileName, childPath));
    }

    Node createdNode = Node(childNode, m_node, childPath, m_fileName, m_fileBuffer, m_usedNodes);

    m_usedNodes->push_back(createdNode);

    return createdNode;
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
            Node nextNode(it->second, node.m_node, childPath, node.m_fileName, node.m_fileBuffer, node.m_usedNodes);
            traverseNode(nextNode, nodes);
        }
    }
    else if (node.m_node.IsSequence()) {
        for (size_t i = 0; i < node.m_node.size(); ++i) {
            std::string childPath = fmt::format("{}[{}]", node.m_path, i);
            Node nextNode(node.m_node[i], node.m_node, childPath, node.m_fileName, node.m_fileBuffer, node.m_usedNodes);
            traverseNode(nextNode, nodes);
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

void Node::checkForUnusedNodes() const
{
    std::list<Node> usedNodes = getUsedNodes();
    std::list<Node> nodes = getNodes();
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
            message += fmt::format("YAML parse error on {}:{}:{}: Unknown keyword '{}'", node.getFilename(), node.getLineInFile(), columnNumber, node.getFullPath());
        }
    }

    if (checkFailed) {
        throw ConfigFileException(message);
    }
}

} // namespace trogondb
