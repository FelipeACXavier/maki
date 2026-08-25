#include "koda_emitter.h"

#include <sstream>

#include "ast/ast.h"
#include "logging.h"
#include "result.h"
#include "types.h"

static const std::string INDENT = "  ";

#define IF_ALT(ALT, OBJ, CALL, ...)     \
  if (std::holds_alternative<ALT>(OBJ)) \
  {                                     \
    if (auto obj = std::get<ALT>(OBJ))  \
      return CALL(*obj, __VA_ARGS__);   \
  }

#define ELSE_IF_ALT(ALT, OBJ, CALL, ...)     \
  else if (std::holds_alternative<ALT>(OBJ)) \
  {                                          \
    if (auto obj = std::get<ALT>(OBJ))       \
      return CALL(*obj, __VA_ARGS__);        \
  }

namespace koda
{
Result<std::string> KodaEmitter::emitKoda(const koda::System& ast)
{
  KodaEmitter emitter;

  ast.print();

  std::stringstream ss;
  for (const auto& component : ast.components)
  {
    if (component->kind != koda::Component::Kind::Capability)
      continue;

    RETURN_ON_FAILURE_AS(emitter.emitCapability(*component, ss), std::string);
  }

  for (const auto& component : ast.components)
  {
    if (component->kind != koda::Component::Kind::Task)
      continue;

    RETURN_ON_FAILURE_AS(emitter.emitTask(*component, ss), std::string);
  }

  // LOG_DEBUG("Koda: {}", ss.str());

  return ss.str();
}

VoidResult KodaEmitter::emitTask(const koda::Component& component, std::stringstream& ss)
{
  ss << "task " << component.name << "(";
  RETURN_ON_FAILURE(emitDefArguments(component.args, ss));
  ss << ") {\n";
  for (const auto& actions : component.statements)
  {
    RETURN_ON_FAILURE(emitStatement(*actions, ss, INDENT));
  }
  ss << "}\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitCapability(const koda::Component& component, std::stringstream& ss)
{
  ss << "capability " << component.name << "(";
  RETURN_ON_FAILURE(emitDefArguments(component.args, ss));
  ss << ") {\n";
  for (const auto& actions : component.statements)
  {
    RETURN_ON_FAILURE(emitStatement(*actions, ss, INDENT));
  }
  ss << "}\n";
  return VoidResult();
}

VoidResult KodaEmitter::emitStatement(const koda::Statement& statement, std::stringstream& ss, const std::string& format)
{
  IF_ALT(PStrategyBlock, statement.node, emitStrategyBlock, ss, format)
  ELSE_IF_ALT(PActionDef, statement.node, emitActionDef, ss, format)
  ELSE_IF_ALT(PRosDef, statement.node, emitRosDef, ss, format)
  ELSE_IF_ALT(PVarsBlock, statement.node, emitVarsBlock, ss, format)

  return VoidResult();
}

VoidResult KodaEmitter::emitStrategyBlock(const koda::StrategyBlock& node, std::stringstream& ss, const std::string& format)
{
  ss << format << "strategy {\n";
  for (auto& flow : node.flows)
  {
    RETURN_ON_FAILURE(emitFlow(*flow, ss, format + INDENT));
  }
  ss << format << "}\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitActionDef(const koda::ActionDef& node, std::stringstream& ss, const std::string& format)
{
  if (node.kind == koda::ActionDef::Kind::Action)
    ss << format << "action";
  else if (node.kind == koda::ActionDef::Kind::Service)
    ss << format << "service";
  else if (node.kind == koda::ActionDef::Kind::Topic)
    ss << format << "topic";

  ss << " \"" << node.label1 << "\" \"" << node.label2 << "\" {\n";
  for (const auto& ros : node.rosDefs)
    RETURN_ON_FAILURE(emitRosDef(*ros, ss, format + INDENT));

  ss << format << "}\n";
  return VoidResult();
}

VoidResult KodaEmitter::emitRosDef(const koda::RosDef& node, std::stringstream& ss, const std::string& format)
{
  if (node.kind == koda::RosDef::Kind::Trigger)
    ss << format << "trigger";
  else if (node.kind == koda::RosDef::Kind::Return)
    ss << format << "return";
  else if (node.kind == koda::RosDef::Kind::Abort)
    ss << format << "abort";
  else if (node.kind == koda::RosDef::Kind::Error)
    ss << format << "error";
  else if (node.kind == koda::RosDef::Kind::In)
    ss << format << "in";
  else if (node.kind == koda::RosDef::Kind::Out)
    ss << format << "out";

  ss << ": " << node.def->typeName << " " << node.def->name + "(";
  RETURN_ON_FAILURE(emitDefArguments(node.def->args, ss));
  ss << ");\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitVarsBlock(const koda::VarsBlock& node, std::stringstream& ss, const std::string& format)
{
  if (node.vars.empty())
    return VoidResult();

  ss << format << "vars {\n";
  for (auto& var : node.vars)
  {
    RETURN_ON_FAILURE(emitVarDef(*var, ss, format + INDENT));
  }
  ss << format << "}\n";
  return VoidResult();
}

VoidResult KodaEmitter::emitVarDef(const koda::VarDef& varDef, std::stringstream& ss, const std::string& format)
{
  ss << format << " "
     << varDef.varType << " "
     << varDef.name << "_ = "
     << varDef.name << " : ";

  RETURN_ON_FAILURE(emitExpression(*varDef.init, ss, format));

  ss << "\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitFlow(const koda::Flow& flow, std::stringstream& ss, const std::string& format)
{
  ss << format << flow.name;
  if (!flow.args.empty())
  {
    ss << "[";
    for (size_t i = 0; i < flow.args.size(); ++i)
    {
      if (i > 0)
        ss << ", ";
      ss << flow.args.at(i)->b;
    }
    ss << "]";
  }
  ss << ": ";
  RETURN_ON_FAILURE(emitStrategy(*flow.strategy, ss, ""));
  ss << ";\n";
  return VoidResult();
}

VoidResult KodaEmitter::emitStrategy(const koda::Strategy& strategy, std::stringstream& ss, const std::string& format)
{
  IF_ALT(PSeq, strategy.v, emitSequence, ss, format)
  ELSE_IF_ALT(PJoin, strategy.v, emitJoin, ss, format)
  ELSE_IF_ALT(PWithin, strategy.v, emitWithin, ss, format)
  ELSE_IF_ALT(PRepeat, strategy.v, emitRepeat, ss, format)
  ELSE_IF_ALT(PEnd, strategy.v, emitEnd, ss, format)
  ELSE_IF_ALT(PContinue, strategy.v, emitContinue, ss, format)
  ELSE_IF_ALT(PRef, strategy.v, emitRef, ss, format)
  ELSE_IF_ALT(PTaskCall, strategy.v, emitTaskCall, ss, format)
  // ELSE_IF_ALT(PParen, strategy.v, emitParen, ss, format)
  // ELSE_IF_ALT(PIfElse, strategy.v, emitIfElse, ss, format)
  // ELSE_IF_ALT(PEither, strategy.v, emitEither, ss, format)
  // ELSE_IF_ALT(PLet, strategy.v, emitLet, ss, format)
  // ELSE_IF_ALT(PGuard, strategy.v, emitGuard, ss, format)

  return VoidResult();
}

VoidResult KodaEmitter::emitSequence(const koda::Strategy::Seq& node, std::stringstream& ss, const std::string& format)
{
  for (size_t i = 0; i < node.alts.size(); ++i)
  {
    if (i != 0)
      ss << " --> ";

    ss << "(";
    RETURN_ON_FAILURE(emitStrategy(*node.alts[i], ss, format));
    ss << ")";
  }

  return VoidResult();
}

VoidResult KodaEmitter::emitJoin(const koda::Strategy::Join& node, std::stringstream& ss, const std::string& format)
{
  ss << "join (";
  for (size_t i = 0; i < node.alts.size(); ++i)
  {
    if (i != 0)
      ss << " | ";

    ss << "(";
    RETURN_ON_FAILURE(emitStrategy(*node.alts[i], ss, format));
    ss << ")";
  }
  ss << ")";

  return VoidResult();
}

VoidResult KodaEmitter::emitWithin(const koda::Strategy::Within& node, std::stringstream& ss, const std::string& format)
{
  ss << "within " << node.seconds << " do (";
  RETURN_ON_FAILURE(emitStrategy(*node.a, ss, format));
  ss << ") else (";
  RETURN_ON_FAILURE(emitStrategy(*node.b, ss, format));
  ss << ")";
  return VoidResult();
}

VoidResult KodaEmitter::emitRepeat(const koda::Strategy::Repeat& node, std::stringstream& ss, const std::string& format)
{
  ss << "repeat " << node.iterations << " " << node.seconds;
  ss << " (";
  RETURN_ON_FAILURE(emitStrategy(*node.a, ss, format));
  ss << ")";
  for (const auto& handler : node.handlers)
    RETURN_ON_FAILURE(emitHandler(*handler, ss, format));

  return VoidResult();
}

VoidResult KodaEmitter::emitEnd(const koda::Strategy::End& node, std::stringstream& ss, const std::string& format)
{
  Q_UNUSED(node)
  Q_UNUSED(format)

  ss << "end";
  return VoidResult();
}

VoidResult KodaEmitter::emitContinue(const koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format)
{
  ss << "continue";
  return VoidResult();
}

VoidResult KodaEmitter::emitRef(const koda::Strategy::Ref& node, std::stringstream& ss, const std::string& format)
{
  ss << node.name;
  return VoidResult();
}

VoidResult KodaEmitter::emitTaskCall(const koda::Strategy::TaskCall& node, std::stringstream& ss, const std::string& format)
{
  RETURN_ON_FAILURE(emitEventCall(*node.call, ss, format));
  for (const auto& handler : node.handlers)
    RETURN_ON_FAILURE(emitHandler(*handler, ss, format));

  return VoidResult();
}

VoidResult KodaEmitter::emitEventCall(const koda::EventCall& node, std::stringstream& ss, const std::string& format)
{
  ss << node.receiver;
  if (!node.name.empty())
    ss << "." << node.name;

  ss << "(";
  RETURN_ON_FAILURE(emitCallArguments(node.args, ss));
  ss << ")";

  return VoidResult();
}

VoidResult KodaEmitter::emitHandler(const koda::StrategyHandler& node, std::stringstream& ss, const std::string& format)
{
  if (node.kind == koda::StrategyHandler::Kind::OnAbort)
  {
    ss << " on abort";
  }
  else if (node.kind == koda::StrategyHandler::Kind::OnError)
  {
    ss << " on error";
  }
  else if (node.kind == koda::StrategyHandler::Kind::OnEmitter)
  {
    ss << " on ";
    RETURN_ON_FAILURE(emitEventCall(*node.emitter, ss, format));
  }

  ss << " (";
  RETURN_ON_FAILURE(emitStrategy(*node.body, ss, format));
  ss << ")";

  return VoidResult();
}

VoidResult KodaEmitter::emitExpression(const koda::Expr& node, std::stringstream& ss, const std::string& format)
{
  IF_ALT(PId, node.v, emitId, ss, format)
  ELSE_IF_ALT(PStr, node.v, emitStr, ss, format)
  ELSE_IF_ALT(PInt, node.v, emitInt, ss, format)
  ELSE_IF_ALT(PFloat, node.v, emitFloat, ss, format)
  ELSE_IF_ALT(PCall, node.v, emitCall, ss, format)
  ELSE_IF_ALT(PNeg, node.v, emitNeg, ss, format)
  ELSE_IF_ALT(PNot, node.v, emitNot, ss, format)
  ELSE_IF_ALT(PBinOp, node.v, emitBinOp, ss, format)
  ELSE_IF_ALT(PEParen, node.v, emitParen, ss, format)

  return VoidResult();
}

VoidResult KodaEmitter::emitId(const koda::Expr::Id& expr, std::stringstream& ss, const std::string& format)
{
  ss << expr.value;
  return VoidResult();
}

VoidResult KodaEmitter::emitStr(const koda::Expr::Str& expr, std::stringstream& ss, const std::string& format)
{
  ss << "\"" << expr.value << "\"";
  return VoidResult();
}

VoidResult KodaEmitter::emitInt(const koda::Expr::Int& expr, std::stringstream& ss, const std::string& format)
{
  ss << expr.value;
  return VoidResult();
}

VoidResult KodaEmitter::emitFloat(const koda::Expr::Float& expr, std::stringstream& ss, const std::string& format)
{
  ss << expr.value;
  return VoidResult();
}

VoidResult KodaEmitter::emitCall(const koda::Expr::Call& expr, std::stringstream& ss, const std::string& format)
{
  return VoidResult();
}

VoidResult KodaEmitter::emitNeg(const koda::Expr::Neg& expr, std::stringstream& ss, const std::string& format)
{
  ss << expr.value;
  return VoidResult();
}

VoidResult KodaEmitter::emitNot(const koda::Expr::Not& expr, std::stringstream& ss, const std::string& format)
{
  return VoidResult();
}

VoidResult KodaEmitter::emitBinOp(const koda::Expr::BinOp& expr, std::stringstream& ss, const std::string& format)
{
  return VoidResult();
}

VoidResult KodaEmitter::emitParen(const koda::Expr::Paren& expr, std::stringstream& ss, const std::string& format)
{
  return VoidResult();
}

VoidResult KodaEmitter::emitCallArguments(const std::vector<std::shared_ptr<Expr>>& args, std::stringstream& ss)
{
  for (size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      ss << ", ";

    RETURN_ON_FAILURE(emitExpression(*args.at(i), ss, ""));
  }

  return VoidResult();
}

VoidResult KodaEmitter::emitDefArguments(const std::vector<std::shared_ptr<Argument>>& args, std::stringstream& ss)
{
  for (size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      ss << ", ";

    const auto& arg = args[i];
    if (arg->kind == koda::Argument::Kind::Req)
      ss << arg->a << " req " << arg->b;
    else
      ss << arg->a << " " << arg->b;
  }

  return VoidResult();
}

}  // namespace koda
