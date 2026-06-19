#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <logging.h>
#include <map>

using json = nlohmann::json;

namespace koda
{


struct MirrorNode {
  struct Span
  {
    int lineStart = 0, colStart = 0;
    int lineEnd = 0, colEnd = 0;

  };
  std::string kind;        // e.g. "Component", "Flow", "EventCall"
  std::string name;        // user‑given name
  std::string srcId;       // the ID you want to trace
  std::string ASTtype;
  std::vector<std::string> tags;
  Span span;

  std::map<std::string, std::vector<MirrorNode>> groups;
  std::map<std::string, std::string> properties;

  // Utility to check if srcId exists
  bool hasSrcId() const { return !srcId.empty(); }
  std::string toString() const;

  const std::vector<MirrorNode>& group(const std::string& name) const;
  std::string prop(const std::string& key) const;
};

MirrorNode loadMirrorAST(const std::string& filepath);
std::vector<MirrorNode> loadComponents(const std::string& filepath);
MirrorNode parseMirrorNode(const json& j);

class MirrorWalker {
  const MirrorNode* current = nullptr;
  std::vector<const MirrorNode*> stack;
public:
  void reset(const MirrorNode& root);
  void setChild(const MirrorNode& current);
  void leave();

  std::string srcId() const {
    return current ? current->srcId : "";
  }
  const MirrorNode* node() const { return current; }
};


}
