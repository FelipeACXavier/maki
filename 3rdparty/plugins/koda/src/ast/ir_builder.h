#pragma once

#include "ast.h"
#include "koda_ir.h"
#include "result.h"
#include "semantic_analyzer.h"
#include "symbol_registry.h"
#include "traceability_map.h"

namespace koda
{
class IRBuilder
{
public:
  IRBuilder(const SymbolRegistry& symbols, const SemanticModel& semantics, std::shared_ptr<TraceabilityMap> traceability);
  Result<ir::Program> build(const System& system) const;

private:
  Result<ir::Component> buildComponent(const PComponent& component) const;
  Result<ir::Flow> buildFlow(const PFlow& flow, SymbolId owner) const;
  Result<ir::PStrategy> buildStrategy(const PStrategy& strategy, SymbolId owner) const;
  Result<ir::PHandler> buildHandler(const PStrategyHandler& handler, SymbolId owner) const;
  Result<ir::Call> buildCall(const PEventCall& call, SymbolId owner) const;
  Result<ir::PExpression> buildExpr(const PExpr& expr, SymbolId owner) const;

  ir::Argument buildArg(const koda::PArgument kodaArg, SymbolId owner) const;
  void appendRosDef(const PRosDef& ros, SymbolId owner, ir::Component& out) const;

  const SymbolRegistry& mSymbols;
  const SemanticModel& mSemantics;
  std::shared_ptr<TraceabilityMap> mTraceMap;
};

}  // namespace koda
