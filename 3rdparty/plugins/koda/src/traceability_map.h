#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace koda
{

enum class MakiElementKind
{
  Node = 0,
  Async,
  Sync,
  Flow
};

struct MakiSource
{
  std::string id;
  std::string flowId;
  MakiElementKind kind = MakiElementKind::Node;
};

class TraceabilityMap
{
public:
  void clear();

  // MAKI -> AST
  void mapAst(const std::string& ast, const MakiSource& source);

  // AST -> IR
  void mapIr(const std::string& ir, const std::string& ast);

  // Later: IR -> Dezyne
  void mapEmitter(const std::string& emitter, const std::string& ir);

  std::optional<MakiSource> sourceForEmitter(const std::string& emitter) const;
  std::vector<std::string> EmitterForSource(const std::string& makiId) const;

  void print() const;

private:
  std::unordered_map<std::string, MakiSource> mAstToMaki;
  std::unordered_map<std::string, std::vector<std::string>> mMakiToAst;

  std::unordered_map<std::string, std::string> mIrToAst;
  std::unordered_map<std::string, std::string> mAstToIr;

  std::unordered_map<std::string, std::string> mEmitterToIr;
  std::unordered_map<std::string, std::vector<std::string>> mIrToEmitter;
};

}  // namespace koda