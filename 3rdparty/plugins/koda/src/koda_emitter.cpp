#include "koda_emitter.h"

#include <sstream>
#include <string>

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

void KodaEmitter::write(std::stringstream& ss, std::string_view text)
{
    ss << text;

    for (char c : text)
    {
        if (c == '\n')
        {
            ++m_line;
            m_col = 1;
        }
        else
        {
            ++m_col;
        }
    }
}

template<typename T>
void KodaEmitter::begin(T& node)
{
    if (!m_emitSpans)
        return;

    // node.span.lineStart = m_line;
    // node.span.colStart = m_col;
}

template<typename T>
void KodaEmitter::end(T& node)
{
    if (!m_emitSpans)
        return;

    // node.span.lineEnd = m_line;
    // node.span.colEnd = m_col;
}

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

  // LOG_DEBUG("Koda: %s", ss.str().c_str());

  return ss.str();
}

VoidResult KodaEmitter::emitTask(const koda::Component& component, std::stringstream& ss)
{
  // ss << "task " << component.name << "(";
  write(ss, "task " + component.name + "(");
  RETURN_ON_FAILURE(emitDefArguments(component.args, ss));
  // ss << ") {\n";
  write(ss, ") {\n");
  for (const auto& actions : component.statements)
  {
    RETURN_ON_FAILURE(emitStatement(*actions, ss, INDENT));
  }
  write(ss, "}\n");
  // ss << "}\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitCapability(const koda::Component& component, std::stringstream& ss)
{
  begin(component);
  write(ss, "capability " + component.name + "(");
  // ss << "capability " << component.name << "(";
  RETURN_ON_FAILURE(emitDefArguments(component.args, ss));
  write(ss, ") {\n");
  // ss << ") {\n";
  for (const auto& actions : component.statements)
  {
    RETURN_ON_FAILURE(emitStatement(*actions, ss, INDENT));
  }
  // ss << "}\n";
  write(ss, "}\n");
  end(component);
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
  write(ss, format + "strategy {\n");
  // ss << format << "strategy {\n";
  for (auto& flow : node.flows)
  {
    RETURN_ON_FAILURE(emitFlow(*flow, ss, format + INDENT));
  }
  write(ss, format + "}\n");
  // ss << format << "}\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitActionDef(const koda::ActionDef& node, std::stringstream& ss, const std::string& format)
{
  if (node.kind == koda::ActionDef::Kind::Action)
    write(ss, format + "action");
    // ss << format << "action";
  else if (node.kind == koda::ActionDef::Kind::Service)
    write(ss, format + "service");
    // ss << format << "service";
  else if (node.kind == koda::ActionDef::Kind::Topic)
    write(ss, format + "topic");
    // ss << format << "topic";

  // ss << " \"" << node.label1 << "\" \"" << node.label2 << "\" {\n";
  write(ss, " \"" + node.label1 + "\" \"" + node.label2 + "\" {\n");
  for (const auto& ros : node.rosDefs)
    RETURN_ON_FAILURE(emitRosDef(*ros, ss, format + INDENT));

  // ss << format << "}\n";
  write(ss, format + "}\n");
  return VoidResult();
}

VoidResult KodaEmitter::emitRosDef(const koda::RosDef& node, std::stringstream& ss, const std::string& format)
{
  if (node.kind == koda::RosDef::Kind::Trigger)
    write(ss, format + "trigger");
    // ss << format << "trigger";
  else if (node.kind == koda::RosDef::Kind::Return)
    write(ss, format + "return");
    // ss << format << "return";
  else if (node.kind == koda::RosDef::Kind::Abort)
    write(ss, format + "abort");
    // ss << format << "abort";
  else if (node.kind == koda::RosDef::Kind::Error)
    write(ss, format + "error");
    // ss << format << "error";
  else if (node.kind == koda::RosDef::Kind::In)
    write(ss, format + "in");
    // ss << format << "in";
  else if (node.kind == koda::RosDef::Kind::Out)
    write(ss, format + "out");
    // ss << format << "out";

  write(ss, ": " + node.def->typeName + " " + node.def->name + "(");
  // ss << ": " << node.def->typeName << " " << node.def->name + "(";
  RETURN_ON_FAILURE(emitDefArguments(node.def->args, ss));
  write(ss, ");\n");
  // ss << ");\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitVarsBlock(const koda::VarsBlock& node, std::stringstream& ss, const std::string& format)
{
  if (node.vars.empty())
    return VoidResult();

  write(ss, format + "vars {\n");
  // ss << format << "vars {\n";
  for (auto& var : node.vars)
  {
    RETURN_ON_FAILURE(emitVarDef(*var, ss, format + INDENT));
  }
  write(ss, format + "}\n");
  // ss << format << "}\n";
  return VoidResult();
}

VoidResult KodaEmitter::emitVarDef(const koda::VarDef& varDef, std::stringstream& ss, const std::string& format)
{
  write(ss, format + " "
     + varDef.varType + " "
     + varDef.name + "_ = "
     + varDef.name + " : ");
  // ss << format << " "
  //    << varDef.varType << " "
  //    << varDef.name << "_ = "
  //    << varDef.name << " : ";

  RETURN_ON_FAILURE(emitExpression(*varDef.init, ss, format));

  write(ss, "\n");
  // ss << "\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitFlow(const koda::Flow& flow, std::stringstream& ss, const std::string& format)
{
  write(ss, format + flow.name);
  // ss << format << flow.name;
  if (!flow.tags.empty())
  {
    write(ss, "[");
    // ss << "[";
    for (size_t i = 0; i < flow.tags.size(); ++i)
    {
      if (i > 0)
        write(ss, ", ");
        // ss << ", ";
      write(ss, flow.tags.at(i));
      // ss << flow.tags.at(i);
    }
    write(ss, "]");
    // ss << "]";
  }
  write(ss, ": ");
  // ss << ": ";
  RETURN_ON_FAILURE(emitStrategy(*flow.strategy, ss, ""));
  write(ss, ";\n");
  // ss << ";\n";
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
      write(ss, " --> ");
      // ss << " --> ";

    write(ss, "(");
    // ss << "(";
    RETURN_ON_FAILURE(emitStrategy(*node.alts[i], ss, format));
    write(ss, ")");
    // ss << ")";
  }

  return VoidResult();
}

VoidResult KodaEmitter::emitJoin(const koda::Strategy::Join& node, std::stringstream& ss, const std::string& format)
{
  write(ss, "join (");
  // ss << "join (";
  for (size_t i = 0; i < node.alts.size(); ++i)
  {
    if (i != 0)
      write (ss, " | ");
      // ss << " | ";

    write(ss, "(");
    // ss << "(";
    RETURN_ON_FAILURE(emitStrategy(*node.alts[i], ss, format));
    write(ss, ")");
    // ss << ")";
  }
  write(ss, ")");
  // ss << ")";

  return VoidResult();
}

VoidResult KodaEmitter::emitWithin(const koda::Strategy::Within& node, std::stringstream& ss, const std::string& format)
{
  write(ss, "within " + node.seconds + std::string(" do ("));
  // ss << "within " << node.seconds << " do (";
  RETURN_ON_FAILURE(emitStrategy(*node.a, ss, format));
  write(ss, ") else (");
  // ss << ") else (";
  RETURN_ON_FAILURE(emitStrategy(*node.b, ss, format));
  write(ss, ")");
  // ss << ")";
  return VoidResult();
}

VoidResult KodaEmitter::emitRepeat(const koda::Strategy::Repeat& node, std::stringstream& ss, const std::string& format)
{
  write(ss, "repeat " + node.iterations + std::string(" ") + std::to_string(node.seconds));
  // ss << "repeat " << node.iterations << " " << node.seconds;
  write(ss, " (");
  // ss << " (";
  RETURN_ON_FAILURE(emitStrategy(*node.a, ss, format));
  write(ss, ")");
  // ss << ")";
  for (const auto& handler : node.handlers)
    RETURN_ON_FAILURE(emitHandler(*handler, ss, format));

  return VoidResult();
}

VoidResult KodaEmitter::emitEnd(const koda::Strategy::End& node, std::stringstream& ss, const std::string& format)
{
  Q_UNUSED(node);
  Q_UNUSED(format);

  write(ss, "end");
  // ss << "end";
  return VoidResult();
}

VoidResult KodaEmitter::emitContinue(const koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format)
{
  write(ss, "continue");
  // ss << "continue";
  return VoidResult();
}

VoidResult KodaEmitter::emitRef(const koda::Strategy::Ref& node, std::stringstream& ss, const std::string& format)
{
  write(ss, node.name);
  // ss << node.name;
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
  write(ss, node.receiver);
  // ss << node.receiver;
  if (!node.name.empty())
    write(ss, "." + node.name);
    // ss << "." << node.name;

  write(ss, "(");
  // ss << "(";
  RETURN_ON_FAILURE(emitCallArguments(node.args, ss));
  write(ss, ")");
  // ss << ")";

  return VoidResult();
}

VoidResult KodaEmitter::emitHandler(const koda::StrategyHandler& node, std::stringstream& ss, const std::string& format)
{
  if (node.kind == koda::StrategyHandler::Kind::OnAbort)
  {
    write(ss, " on abort");
    // ss << " on abort";
  }
  else if (node.kind == koda::StrategyHandler::Kind::OnError)
  {
    write(ss, " on error");
    // ss << " on error";
  }
  else if (node.kind == koda::StrategyHandler::Kind::OnEmitter)
  {
    write(ss, " on ");
    // ss << " on ";
    RETURN_ON_FAILURE(emitEventCall(*node.emitter, ss, format));
  }

  write(ss, " (");
  // ss << " (";
  RETURN_ON_FAILURE(emitStrategy(*node.body, ss, format));
  write(ss, ")");
  // ss << ")";

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
  // ss << expr.value;
  write(ss, expr.value);
  return VoidResult();
}

VoidResult KodaEmitter::emitStr(const koda::Expr::Str& expr, std::stringstream& ss, const std::string& format)
{
  // ss << "\"" << expr.value << "\"";
  write(ss, "\"" + expr.value + "\"");
  return VoidResult();
}

VoidResult KodaEmitter::emitInt(const koda::Expr::Int& expr, std::stringstream& ss, const std::string& format)
{
  // ss << expr.value;
  write(ss, "" + expr.value);
  return VoidResult();
}

VoidResult KodaEmitter::emitFloat(const koda::Expr::Float& expr, std::stringstream& ss, const std::string& format)
{
  // ss << expr.value;
  write(ss, std::to_string(expr.value));
  return VoidResult();
}

VoidResult KodaEmitter::emitCall(const koda::Expr::Call& expr, std::stringstream& ss, const std::string& format)
{
  return VoidResult();
}

VoidResult KodaEmitter::emitNeg(const koda::Expr::Neg& expr, std::stringstream& ss, const std::string& format)
{
  std::stringstream ssOut("");
  ssOut << expr.value;
  std::string out;
  ssOut >> out;
  write(ss, out);
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
      // ss << ", ";
      write(ss, ", ");

    RETURN_ON_FAILURE(emitExpression(*args.at(i), ss, ""));
  }

  return VoidResult();
}

VoidResult KodaEmitter::emitDefArguments(const std::vector<std::shared_ptr<Argument>>& args, std::stringstream& ss)
{
  for (size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      // ss << ", ";
      write(ss, ", ");

    const auto& arg = args[i];
    if (arg->kind == koda::Argument::Kind::Req)
      write(ss, arg->a + " req " + arg->b);
      // ss << arg->a << " req " << arg->b;
    else
      write(ss, arg->a + " " + arg->b);
      // ss << arg->a << " " << arg->b;
  }

  return VoidResult();
}

}  // namespace koda
