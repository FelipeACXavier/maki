#include "traceability_map.h"

#include "logging.h"

namespace koda
{

void TraceabilityMap::clear()
{
  mAstToMaki.clear();
  mMakiToAst.clear();
}

void TraceabilityMap::mapAst(const std::string& astId, const MakiSource& source)
{
  if (astId.empty() || source.id.empty())
    return;

  mMakiToAst[source.id].push_back(astId);
  mAstToMaki[astId] = source;
}

void TraceabilityMap::mapIr(const std::string& ir, const std::string& ast)
{
  if (ir.empty() || ast.empty())
    return;

  mAstToIr[ast] = ir;
  mIrToAst[ir] = ast;
}

void TraceabilityMap::mapEmitter(const std::string& emitter, const std::string& ir)
{
  if (ir.empty() || emitter.empty())
    return;

  mIrToEmitter[ir].push_back(emitter);
  mEmitterToIr[emitter] = ir;
}

void TraceabilityMap::print() const
{
  LOG_INFO("MAKI to AST:");
  for (const auto& [key, value] : mAstToMaki)
    LOG_INFO("  Node: {} Flow: {} <-> {} (Type: {})", value.id, value.flowId, key, (int)value.kind);

  LOG_INFO("AST to IR:");
  for (const auto& [key, value] : mIrToAst)
    LOG_INFO("  {} <-> {}", value, key);

  LOG_INFO("IR to Emitter:");
  for (const auto& [key, value] : mEmitterToIr)
    LOG_INFO("  {} <-> {}", value, key);
}

std::optional<MakiSource> TraceabilityMap::sourceForEmitter(const std::string& emitter) const
{
  // Let's go
  // Emitter -> IR
  const auto ir = mEmitterToIr.find(emitter);
  if (ir == mEmitterToIr.end())
    return std::nullopt;

  // IR -> AST
  const auto ast = mIrToAst.find(ir->second);
  if (ast == mIrToAst.end())
    return std::nullopt;

  // AST -> MAKI
  const auto maki = mAstToMaki.find(ast->second);
  if (maki == mAstToMaki.end())
    return std::nullopt;

  return maki->second;
}

std::vector<std::string> TraceabilityMap::EmitterForSource(const std::string& makiId) const
{
  const auto it = mMakiToAst.find(makiId);
  if (it == mMakiToAst.end())
    return {};

  return it->second;
}

}  // namespace koda