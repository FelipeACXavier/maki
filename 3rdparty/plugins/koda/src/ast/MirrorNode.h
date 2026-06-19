#pragma once

#include <string>
#include <vector>


namespace koda
{
struct MirrorNode {
  std::string kind;        // e.g. "Component", "Flow", "EventCall"
  std::string name;        // user‑given name
  std::string srcId;       // the ID you want to trace
  std::vector<MirrorNode> children;

  // Utility to check if srcId exists
  bool hasSrcId() const { return !srcId.empty(); }
};

class MirrorWalker {
  const MirrorNode* current = nullptr;
  std::vector<const MirrorNode*> stack;
public:
  void reset(const MirrorNode& root) {
    current = &root;
    stack.clear();
  }
  void enterChild(size_t index) {
    if (!current) return;
    stack.push_back(current);
    current = (index < current->children.size())
              ? &current->children[index]
              : nullptr;
  }
  void leave() {
    if (!stack.empty()) {
      current = stack.back();
      stack.pop_back();
    }
  }
  std::string srcId() const {
    return current ? current->srcId : "";
  }
  const MirrorNode* node() const { return current; }
};


}
