#pragma once

#include <sstream>
#include <string_view>

#include "ast/ast.h"
#include "result.h"

namespace koda
{
class KodaEmitter
{
public:
  static Result<std::string> emitKoda(koda::System& ast);

private:
  bool m_emitSpans = true;
  int m_line = 1;
  int m_col = 1;
  char m_lastChar = '\0';
  void write(std::stringstream& ss, std::string_view text);
  template<typename T> void begin(T& node);

  template<typename T> void end(T& node);

  VoidResult emitTask(koda::Component& component, std::stringstream& ss);
  VoidResult emitCapability(koda::Component& component, std::stringstream& ss);

  VoidResult emitStatement(koda::Statement& component, std::stringstream& ss, const std::string& format);
  VoidResult emitStrategyBlock(koda::StrategyBlock& node, std::stringstream& ss, const std::string& format);
  VoidResult emitActionDef(koda::ActionDef& node, std::stringstream& ss, const std::string& format);
  VoidResult emitRosDef(koda::RosDef& node, std::stringstream& ss, const std::string& format);
  VoidResult emitVarsBlock(koda::VarsBlock& node, std::stringstream& ss, const std::string& format);

  VoidResult emitVarDef(koda::VarDef& varDef, std::stringstream& ss, const std::string& format);

  VoidResult emitFlow(koda::Flow& flow, std::stringstream& ss, const std::string& format);
  VoidResult emitStrategy(koda::Strategy& strategy, std::stringstream& ss, const std::string& format);
  VoidResult emitSequence(koda::Strategy::Seq& node, std::stringstream& ss, const std::string& format);
  VoidResult emitJoin(koda::Strategy::Join& node, std::stringstream& ss, const std::string& format);
  VoidResult emitWithin(koda::Strategy::Within& node, std::stringstream& ss, const std::string& format);
  VoidResult emitRepeat(koda::Strategy::Repeat& node, std::stringstream& ss, const std::string& format);
  VoidResult emitEnd(koda::Strategy::End& node, std::stringstream& ss, const std::string& format);
  VoidResult emitContinue(koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format);
  VoidResult emitRef(koda::Strategy::Ref& node, std::stringstream& ss, const std::string& format);
  VoidResult emitTaskCall(koda::Strategy::TaskCall& node, std::stringstream& ss, const std::string& format);

  VoidResult emitEventCall(koda::EventCall& node, std::stringstream& ss, const std::string& format);
  VoidResult emitHandler(koda::StrategyHandler& node, std::stringstream& ss, const std::string& format);

  VoidResult emitExpression(koda::Expr& node, std::stringstream& ss, const std::string& format);
  VoidResult emitId(koda::Expr::Id& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitStr(koda::Expr::Str& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitInt(koda::Expr::Int& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitFloat(koda::Expr::Float& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitCall(koda::Expr::Call& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitNeg(koda::Expr::Neg& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitNot(koda::Expr::Not& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitBinOp(koda::Expr::BinOp& expr, std::stringstream& ss, const std::string& format);
  VoidResult emitParen(koda::Expr::Paren& expr, std::stringstream& ss, const std::string& format);

  VoidResult emitCallArguments(std::vector<std::shared_ptr<Expr>>& args, std::stringstream& ss);
  VoidResult emitDefArguments(std::vector<std::shared_ptr<Argument>>& args, std::stringstream& ss);
};
}  // namespace koda