#pragma once

#include <memory>
#include <list>
#include <vector>
#include <sstream>
#include <optional>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include "trogondb/exception.h"

namespace trogondb {

class ConfigFileException : public Exception {
public:
    using Exception::Exception;
};

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

    std::list<Node> getNodes() const;

    static void checkForUnusedNodes(const Node &root);

private:
    Node(const YAML::Node &node,
         const std::optional<YAML::Node> &parent,
         const std::string &path,
         const std::string &filename,
         const std::shared_ptr<std::stringstream> &fileBuffer,
         const std::shared_ptr<std::list<Node>> &usedNodes);

    std::list<Node> getUsedNodes() const;

    static void traverseNode(const Node &root, std::list<Node> *outputNodes);

    static std::string getLastElementInNodePath(const std::string &path);

private:
    YAML::Node m_node;
    std::optional<YAML::Node> m_parent;
    std::string m_path;
    std::string m_fileName;
    std::shared_ptr<std::stringstream> m_fileBuffer;
    mutable std::shared_ptr<std::list<Node>> m_usedNodes;
};

template <typename T>
T Node::getValue(const std::string &key) const
{
    std::string childPath = m_path.empty() ? key : fmt::format("{}.{}", m_path, key);

    YAML::Node childNode = m_node[key];

    if (!childNode) {
        throw ConfigFileException(fmt::format("YAML parse error on {}: Missing keyword '{}'", m_fileName, childPath));
    }

    try {
        m_usedNodes->push_back(Node(childNode, m_node, childPath, m_fileName, m_fileBuffer, m_usedNodes));
        return childNode.as<T>();
    }
    catch (const YAML::TypedBadConversion<T> &e) {
        int lineNumber = e.mark.line + 1;
        int columnNumber = e.mark.column + 1;
        std::string valueAsString = childNode.as<std::string>();
        throw ConfigFileException(fmt::format("YAML parse error on  {}:{}:{}: Invalid value '{}' for keyword '{}'", m_fileName, lineNumber, columnNumber, valueAsString, childPath));
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
        throw ConfigFileException(fmt::format("YAML parse error on {}:{}:{}: Invalid value '{}' for keyword '{}'", m_fileName, line, column, valueAsString, childPath));
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
        throw ConfigFileException(fmt::format("YAML parse error on {}:{}:{}: Invalid value '{}' for keyword '{}'", m_fileName, lineNumber, columnNumber, valueAsString, path));
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
        throw ConfigFileException(fmt::format("YAML parse error on {}:{}:{}: Invalid value '{}' for keyword '{}'", m_fileName, lineNumber, columnNumber, valueAsString, path));
    }
    return value;
}

} // namespace trogondb
