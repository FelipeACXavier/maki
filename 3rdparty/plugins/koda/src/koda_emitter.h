#pragma once

#include <set>
#include <sstream>

#include "ast/ast.h"
#include "result.h"
#include "typing/type_definition.h"
#include "typing/type_registry.h"

namespace koda
{
class KodaEmitter
{
public:
  KodaEmitter(const koda::types::TypeRegistry* registry);

  static Result<std::string> emitKoda(const koda::System& ast, const koda::types::TypeRegistry* registry);

private:
  VoidResult emitTask(const koda::Component& component, std::stringstream& ss);
  VoidResult emitCapability(const koda::Component& component, std::stringstream& ss);
  VoidResult emitTypes(std::stringstream& ss);
  Result<std::string> emitTypeReference(const koda::types::TypeReference& reference) const;

  VoidResult emitType(const koda::types::TypeDefinition& definition, std::stringstream& ss, const std::string& format = "");
  VoidResult emitTypeRecursive(const koda::types::TypeDefinition& definition, std::stringstream& ss, std::set<std::string>& emitted,
                               std::set<std::string>& visiting);
  void collectTypeDependencies(const koda::types::TypeReference& reference, std::vector<koda::types::TypeReference>& out) const;
  std::vector<koda::types::TypeReference> typeDependencies(const koda::types::TypeDefinition& definition) const;

  VoidResult emitStatement(const koda::Statement& component, std::stringstream& ss, const std::string& format);
  VoidResult emitStrategyBlock(const koda::StrategyBlock& node, std::stringstream& ss, const std::string& format);
  VoidResult emitActionDef(const koda::ActionDef& node, std::stringstream& ss, const std::string& format);
  VoidResult emitRosDef(const koda::RosDef& node, std::stringstream& ss, const std::string& format);
  VoidResult emitVarsBlock(const koda::VarsBlock& node, std::stringstream& ss, const std::string& format);

  VoidResult emitVarDef(const koda::VarDef& varDef, std::stringstream& ss, const std::string& format);

  VoidResult emitFlow(const koda::Flow& flow, std::stringstream& ss, const std::string& format);
  VoidResult emitStrategy(const koda::Strategy& strategy, std::stringstream& ss, const std::string& format);
  VoidResult emitSequence(const koda::Strategy::Seq& node, std::stringstream& ss, const std::string& format);
  VoidResult emitJoin(const koda::Strategy::Join& node, std::stringstream& ss, const std::string& format);
  VoidResult emitWithin(const koda::Strategy::Within& node, std::stringstream& ss, const std::string& format);
  VoidResult emitRepeat(const koda::Strategy::Repeat& node, std::stringstream& ss, const std::string& format);
  VoidResult emitEnd(const koda::Strategy::End& node, std::stringstream& ss, const std::string& format);
  VoidResult emitContinue(const koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format);
  VoidResult emitTaskCall(const koda::Strategy::TaskCall& node, std::stringstream& ss, const std::string& format);

  VoidResult emitEventCall(const koda::EventCall& node, std::stringstream& ss, const std::string& format);
  VoidResult emitHandler(const koda::StrategyHandler& node, std::stringstream& ss, const std::string& format);

  VoidResult emitExpression(const koda::Expr& node, std::stringstream& ss, const std::string& format);
  VoidResult emitId(const koda::Expr::Id& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitStr(const koda::Expr::Str& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitInt(const koda::Expr::Int& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitFloat(const koda::Expr::Float& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitBool(const koda::Expr::Bool& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitCall(const koda::Expr::Call& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitNeg(const koda::Expr::Neg& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitNot(const koda::Expr::Not& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitBinOp(const koda::Expr::BinOp& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitParen(const koda::Expr::Paren& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitRecordLiteral(const koda::Expr::RecordLiteral& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitListLiteral(const koda::Expr::ListLiteral& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitMapLiteral(const koda::Expr::MapLiteral& expr, std::stringstream& ss, const std::string& format);

  VoidResult emitCallArguments(const std::vector<std::shared_ptr<Expr>>& args, std::stringstream& ss);
  VoidResult emitDefArguments(const std::vector<std::shared_ptr<Argument>>& args, std::stringstream& ss);

private:
  const koda::types::TypeRegistry* mTypeRegistry;
};
}  // namespace koda