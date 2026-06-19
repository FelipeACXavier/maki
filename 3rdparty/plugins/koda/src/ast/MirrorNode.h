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
}
