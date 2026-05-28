#pragma once

#include <sstream>

#include "ast/ast.h"
#include "result.h"

namespace koda
{
class KodaEmitter
{
public:
  static Result<std::string> emitKoda(const koda::System& ast);

private:
  VoidResult emitTask(const koda::Component& component, std::stringstream& ss);
  VoidResult emitCapability(const koda::Component& component, std::stringstream& ss);

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
  VoidResult emitEvery(const koda::Strategy::Every& node, std::stringstream& ss, const std::string& format);
  VoidResult emitEnd(const koda::Strategy::End& node, std::stringstream& ss, const std::string& format);
  VoidResult emitContinue(const koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format);
  VoidResult emitRef(const koda::Strategy::Ref& node, std::stringstream& ss, const std::string& format);
  VoidResult emitTaskCall(const koda::Strategy::TaskCall& node, std::stringstream& ss, const std::string& format);

  VoidResult emitEventCall(const koda::EventCall& node, std::stringstream& ss, const std::string& format);
  VoidResult emitHandler(const koda::StrategyHandler& node, std::stringstream& ss, const std::string& format);

  VoidResult emitExpression(const koda::Expr& node, std::stringstream& ss, const std::string& format);
  VoidResult emitId(const koda::Expr::Id& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitStr(const koda::Expr::Str& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitInt(const koda::Expr::Int& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitFloat(const koda::Expr::Float& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitCall(const koda::Expr::Call& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitNeg(const koda::Expr::Neg& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitNot(const koda::Expr::Not& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitBinOp(const koda::Expr::BinOp& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitParen(const koda::Expr::Paren& expr, std::stringstream& ss, const std::string& format);

  VoidResult emitCallArguments(const std::vector<std::shared_ptr<Expr>>& args, std::stringstream& ss);
  VoidResult emitDefArguments(const std::vector<std::shared_ptr<Argument>>& args, std::stringstream& ss);
};
}  // namespace koda