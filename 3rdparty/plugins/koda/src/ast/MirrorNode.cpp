#include "MirrorNode.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <execinfo.h>   // for backtrace, backtrace_symbols (Linux, macOS)
#include <cstdlib>
#include <sstream>

using json = nlohmann::json;

namespace koda
{

void logStackTrace() {
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    std::ostringstream oss;
    oss << "Stack trace (most recent call first):\n";
    for (int i = 0; i < frames; ++i) {
        oss << strs[i] << '\n';
    }
    free(strs);
    LOG_ERROR("%s", oss.str().c_str());
}

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

  const std::vector<MirrorNode>& MirrorNode::group(const std::string& name) const
  {
    static const std::vector<MirrorNode> empty;
    auto it = groups.find(name);
    return it != groups.end() ? it->second : empty;
  }

  std::string MirrorNode::prop(const std::string& key) const
  {
    std::string def("");
    auto it = properties.find(key);
    return it != properties.end() ? it->second : def;
  }

  static void addGroupIfArray(const json& j, MirrorNode& node, const std::string& key) {
    if (j.contains(key) && j[key].is_array()) {
      auto& vec = node.groups[key];
      for (auto& item : j[key])
        vec.push_back(parseMirrorNode(item));
    }
  }
  static void addGroupIfObject(const json& j, MirrorNode& node, const std::string& key) {
    if (j.contains(key) && j[key].is_object())
      node.groups[key].push_back(parseMirrorNode(j[key]));
  }

  MirrorNode parseMirrorNode(const json& j) {
    MirrorNode node;
    if (j.contains("kind"))  node.kind = j["kind"].get<std::string>();
    if (j.contains("name"))  node.name = j["name"].get<std::string>();
    if (j.contains("srcId")) node.srcId = j["srcId"].get<std::string>();
    if (j.contains("ASTtype")) node.ASTtype = j["ASTtype"].get<std::string>();

    if (j.contains("tags") && j["tags"].is_array())
    {
      for (const auto& tag : j["tags"])
        node.tags.push_back(tag.get<std::string>());
    }

    if (j.contains("span"))
    {
      node.span.lineStart = j["span"]["lineStart"].get<int>();
      node.span.colStart  = j["span"]["colStart"].get<int>();
      node.span.lineEnd   = j["span"]["lineEnd"].get<int>();
      node.span.colEnd    = j["span"]["colEnd"].get<int>();
    }

    const std::string& t = node.ASTtype;
    if (t == "System")
    {
      addGroupIfArray(j, node, "components");
    } else if (t == "Component")
    {
      addGroupIfArray(j, node, "args");
      addGroupIfArray(j, node, "statements");
    } else if (t == "Argument")
    {
      node.properties["a"] = j["a"].get<std::string>();
      node.properties["b"] = j["b"].get<std::string>();
    } else if (t == "Statement")
    {
      addGroupIfObject(j, node, "node");
    } else if (t == "Flow")
    {
      addGroupIfObject(j, node, "strategy");
    } else if (t == "VarDef")
    {
      node.properties["varType"] = j["varType"].get<std::string>();
      addGroupIfObject(j, node, "init");
      addGroupIfObject(j, node, "fallback");
    } else if (t == "EventDefComponent")
    {
      node.properties["text"] = j["text"].get<std::string>();
    } else if (t == "EventDef")
    {
      node.properties["typeName"] = j["typeName"].get<std::string>();
      addGroupIfArray(j, node, "args");
      addGroupIfArray(j, node, "components");
    } else if (t == "RosDef")
    {
      addGroupIfObject(j, node, "def");
    } else if (t == "ActionDef")
    {
      node.properties["label1"] = j["label1"].get<std::string>();
      node.properties["label2"] = j["label2"].get<std::string>();
      addGroupIfArray(j, node, "rosDefs");
    } else if (t == "StrategyBlock")
    {
      addGroupIfArray(j, node, "flows");
    } else if (t == "VarsBlock")
    {
      addGroupIfArray(j, node, "vars");
    } else if (t == "StrategyHandler")
    {
      addGroupIfObject(j, node, "emitter");
      addGroupIfObject(j, node, "body");
    } else if (t == "Strategy")
    {
      addGroupIfObject(j, node, "v");
    } else if (t == "Strategy::Seq" || t == "Strategy::Join" || t == "Strategy::Either")
    {
      addGroupIfArray(j, node, "alts");
    } else if (t == "Strategy::Let")
    {
      addGroupIfObject(j, node, "call");
    } else if (t == "Strategy::Within")
    {
      node.properties["seconds"] = std::to_string(j["seconds"].get<int>());
      addGroupIfObject(j, node, "a");
      addGroupIfObject(j, node, "b");
      addGroupIfArray(j, node, "handlers");
    } else if (t == "Strategy::IfElse")
    {
      addGroupIfObject(j, node, "cond");
      addGroupIfObject(j, node, "a");
      addGroupIfObject(j, node, "b");
    } else if (t == "Strategy::Repeat")
    {
      node.properties["seconds"] = std::to_string(j["seconds"].get<int>());
      node.properties["iterations"] = std::to_string(j["iterations"].get<int>());
      addGroupIfObject(j, node, "a");
      addGroupIfArray(j, node, "handlers");
    } else if (t == "Strategy::Guard")
    {
      addGroupIfObject(j, node, "cond");
    } else if (t == "Strategy::End" || t == "Strategy::Continue" || t == "Strategy::Ref")
    {
      // no additional fields
    } else if (t == "Strategy::TaskCall")
    {
      addGroupIfObject(j, node, "call");
      addGroupIfArray(j, node, "handlers");
    } else if (t == "Strategy::Paren")
    {
      addGroupIfObject(j, node, "a");
    } else if (t == "EventCall")
    {
      node.properties["receiver"] = j["receiver"].get<std::string>();
    } else if (t == "Expr")
    {
      addGroupIfObject(j, node, "v");
    } else if ("Expr::Id" == t || "Expr::Str" == t)
    {
      node.properties["value"] = j["value"].get<std::string>();
    } else if ("Expr::Int" == t)
    {
      node.properties["value"] = std::to_string(j["value"].get<int>());
    } else if ("Expr::Float" == t)
    {
      node.properties["value"] = std::to_string(j["value"].get<double>());
    } else if (t == "Expr::Call" || t == "Expr::Neg" || t == "Expr::Not")
    {
      addGroupIfObject(j, node, "value");
    } else if (t == "Expr::BinOp")
    {
      node.properties["operation"] = j["operation"].get<std::string>();
      addGroupIfObject(j, node, "a");
      addGroupIfObject(j, node, "b");
    } else if (t == "Expr::Paren")
    {
      addGroupIfObject(j, node, "value");
    } else
    {
      LOG_ERROR("Unknown ASTtype: %s", t.c_str());
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

  /**
   * Safely retrieves the first child of a named group.
   * If the group is missing or empty, logs a descriptive error with a stack trace
   * and returns nullptr.
   */
   const MirrorNode* safeChild(const MirrorNode& node,
                                           const std::string& groupName, int pos) {
      auto it = node.groups.find(groupName);
      // also check whether item at pos exists
      if (it == node.groups.end() || it->second.empty() || it->second.size() <= pos) {
          LOG_ERROR(
              "MirrorNode '%s' (type '%s') has no non-empty group '%s', or at position %d",
              node.name.c_str(), node.ASTtype.c_str(), groupName.c_str(), pos
          );
          logStackTrace();       // <-- this dumps the call stack
          return nullptr;
      }
      return &it->second[pos];
  }

  // MirrorWalker class definitions
  void MirrorWalker::reset(const MirrorNode& root)
  {
    current = &root;
    stack.clear();
  }

  void MirrorWalker::setChild(const MirrorNode& node)
  {
    stack.push_back(current);
    current = &node;
  }

  void MirrorWalker::leave() {
    if (!stack.empty()) {
      current = stack.back();
      stack.pop_back();
    }
  }


}