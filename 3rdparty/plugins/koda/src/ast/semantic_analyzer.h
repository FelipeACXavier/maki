#pragma once

#include <unordered_map>
#include <vector>

#include "ast.h"
#include "result.h"
#include "symbol_registry.h"
#include "typing/type_registry.h"

namespace koda
{
enum class ResolvedCallKind
{
  Unknown = 0,
  CapabilityTrigger,
  Event
};

struct ResolvedCall
{
  ResolvedCallKind kind = ResolvedCallKind::Unknown;
  SymbolId receiver = InvalidSymbol;  // argument/component instance symbol
  SymbolId target = InvalidSymbol;    // capability or event symbol
  types::TypeReference returnType;
};

struct SemanticModel
{
  std::unordered_map<const EventCall*, ResolvedCall> calls;
  std::unordered_map<const Strategy*, SymbolId> flowRefs;
  std::unordered_map<const Expr*, types::TypeReference> expressionTypes;
  std::unordered_map<SymbolId, std::vector<types::TypeReference>> eventArguments;
};

class SemanticAnalyzer
{
public:
  SemanticAnalyzer(SymbolRegistry& symbols, types::TypeRegistry& types);

  VoidResult run(const System& system);

  const SemanticModel& model() const
  {
    return mModel;
  }

private:
  VoidResult analyzeComponent(const PComponent& component);
  VoidResult analyzeStatement(const PStatement& statement, SymbolId owner);
  VoidResult analyzeStrategy(const PStrategy& strategy, SymbolId owner);
  VoidResult analyzeHandler(const PStrategyHandler& handler, SymbolId owner);
  Result<ResolvedCall> resolveCall(const PEventCall& call, SymbolId owner);
  Result<types::TypeReference> analyzeExpr(const PExpr& expr, SymbolId owner);

  VoidResult collectEventSignatures(const System& system);
  VoidResult collectRosSignature(const PRosDef& ros, SymbolId owner);

  Result<SymbolId> resolveValue(const std::string& name, SymbolId owner, const Span& span) const;
  Result<SymbolId> resolveComponentType(const Symbol& value, const Span& span) const;

  SymbolRegistry& mSymbols;
  types::TypeRegistry& mTypeRegistry;
  SemanticModel mModel;
};

}  // namespace koda
