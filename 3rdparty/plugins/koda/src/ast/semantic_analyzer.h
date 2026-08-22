#pragma once

#include <set>
#include <unordered_map>
#include <vector>

#include "ast.h"
#include "result.h"
#include "symbol_registry.h"
#include "typing/blackboard_model.h"
#include "typing/type_registry.h"

namespace koda
{
enum class ResolvedCallKind
{
  Unknown = 0,
  CapabilityTrigger,
  Event,
  Flow
};

enum class ArgumentSourceKind
{
  Literal,
  Blackboard,
  Infer
};

struct ResolvedArgumentSource
{
  ArgumentSourceKind kind;
  std::optional<koda::types::SlotId> slot;
};

struct ResolvedCall
{
  ResolvedCallKind kind = ResolvedCallKind::Unknown;
  SymbolId receiver = InvalidSymbol;  // argument/component instance symbol
  SymbolId target = InvalidSymbol;    // capability or event symbol
  types::TypeReference returnType;
  std::vector<std::string> args = {};
};

struct SemanticModel
{
  std::unordered_map<const EventCall*, ResolvedCall> calls;
  std::unordered_map<const Strategy*, SymbolId> flowRefs;
  std::unordered_map<const Expr*, types::TypeReference> expressionTypes;
  std::unordered_map<SymbolId, std::vector<types::TypeReference>> eventArguments;
  std::unordered_map<SymbolId, koda::types::SlotId> variableSlots;
};

using FlowArgumentContext = std::map<SymbolId, ResolvedArgumentSource>;

class SemanticAnalyzer
{
public:
  SemanticAnalyzer(SymbolRegistry& symbols, types::TypeRegistry& types, types::Blackboard& blackboard);

  VoidResult run(const System& system);

  const SemanticModel& model() const
  {
    return mModel;
  }

private:
  SemanticModel mModel;
  SymbolRegistry& mSymbols;
  types::Blackboard& mBlackboard;
  types::TypeRegistry& mTypeRegistry;
  std::unordered_map<SymbolId, PFlow> mFlows;
  std::set<SymbolId> mActiveFlows;
  std::vector<FlowArgumentContext> mFlowArgumentStack;

  VoidResult analyzeFlow(SymbolId flowId);
  VoidResult analyzeComponent(const PComponent& component);
  VoidResult analyzeStatement(const PStatement& statement, SymbolId owner);
  VoidResult analyzeStrategy(const PStrategy& strategy, SymbolId owner);
  VoidResult analyzeFlowCall(const PEventCall& astCall, const ResolvedCall& call, SymbolId callerOwner);
  VoidResult analyzeHandler(const PStrategyHandler& handler, SymbolId owner);
  Result<ResolvedCall> resolveCall(const PEventCall& call, SymbolId owner);
  Result<types::TypeReference> analyzeExpr(const PExpr& expr, SymbolId owner, const types::TypeReference& expected = {});

  VoidResult collectEventSignatures(const System& system);
  VoidResult collectRosSignature(const PRosDef& ros, SymbolId owner);

  Result<SymbolId> resolveValue(const std::string& name, SymbolId owner, const Span& span) const;
  Result<SymbolId> resolveComponentType(const Symbol& value, const Span& span) const;

  VoidResult resolveCapabilityData(const PEventCall& astCall, const ResolvedCall& call, SymbolId owner);
  Result<ResolvedArgumentSource> resolveArgumentSource(const PExpr& expr, const types::TypeReference& expectedType, SymbolId owner);

  bool compatible(const types::TypeReference& expected, const types::TypeReference& actual);
};

}  // namespace koda
