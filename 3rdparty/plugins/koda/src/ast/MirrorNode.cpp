#include "MirrorNode.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

using json = nlohmann::json;

namespace koda
{
  std::string MirrorNode::toString() const
  {
    std::stringstream ss;
    std::string out;
    ss << "MirrorNode(";
    ss << ASTtype;
    ss << ", k: ";
    ss << kind;
    ss << ", n: ";
    ss << name;
    ss << ", id: ";
    ss << srcId;
    ss << ")";
    ss >> out;
    return out;
  };

  MirrorNode parseMirrorNode(const json& j) {
    MirrorNode node;
    if (j.contains("kind"))  node.kind = j["kind"].get<std::string>();
    if (j.contains("name"))  node.name = j["name"].get<std::string>();
    if (j.contains("srcId")) node.srcId = j["srcId"].get<std::string>();
    if (j.contains("ASTtype")) node.ASTtype = j["ASTtype"].get<std::string>();

    // If the JSON has a "children" array, recurse
    if (j.contains("children") && j["children"].is_array()) {
      for (const auto& child : j["children"]) {
        LOG_DEBUG("CHILD");
        node.children.push_back(parseMirrorNode(child));
      }
    }
    return node;
  }

  MirrorNode loadMirrorAST(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open())
      throw std::runtime_error("Cannot open " + filepath);

    json j;
    f >> j;
    return parseMirrorNode(j);
  }

  std::vector<MirrorNode> loadComponents(const std::string& filepath) {
    json j;
    std::ifstream(filepath) >> j;
    std::vector<MirrorNode> components;
    if (j.is_array()) {
      for (const auto& item : j)
        components.push_back(parseMirrorNode(item));
    } else {
      // single object – maybe the root is a "system" with "children"
      components.push_back(parseMirrorNode(j));
    }
    return components;
  }
}