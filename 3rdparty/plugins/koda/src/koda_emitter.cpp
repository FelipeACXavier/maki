#include "koda_emitter.h"

#include <sstream>
#include <string>
#include <ios_base>

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
void KodaEmitter::begin(T& node, std::stringstream& ss)
{
    if (!m_emitSpans)
        return;

    node.span.lineStart = m_line;
    node.span.colStart = m_col;
}

template<typename T>
void KodaEmitter::end(T& node, std::stringstream& ss)
{
    if (!m_emitSpans)
        return;
    ss.seekp(-1, std::ios_base::end);
    if (ss.peek() == '\n')
    {
      node.span.lineEnd = m_line - 1; // Need to subtract one since the "\n" is inserted before end is called.
      LOG_DEBUG("END OF LINE");
    }
    else
    {
      ss.seekp( 0, std::ios_base::end );
      node.span.lineEnd = m_line;
    }

    node.span.colEnd = m_col;
}

Result<std::string> KodaEmitter::emitKoda(koda::System& ast)
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

VoidResult KodaEmitter::emitTask(koda::Component& component, std::stringstream& ss)
{
  // ss << "task " << component.name << "(";
  begin(component, ss);
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
  end(component, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitCapability(koda::Component& component, std::stringstream& ss)
{
  begin(component, ss);
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
  end(component, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitStatement(koda::Statement& statement, std::stringstream& ss, const std::string& format)
{
  begin(statement, ss);
  IF_ALT(PStrategyBlock, statement.node, emitStrategyBlock, ss, format)
  ELSE_IF_ALT(PActionDef, statement.node, emitActionDef, ss, format)
  ELSE_IF_ALT(PRosDef, statement.node, emitRosDef, ss, format)
  ELSE_IF_ALT(PVarsBlock, statement.node, emitVarsBlock, ss, format)
  end(statement, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitStrategyBlock(koda::StrategyBlock& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  write(ss, format + "strategy {\n");
  // ss << format << "strategy {\n";
  for (auto& flow : node.flows)
  {
    RETURN_ON_FAILURE(emitFlow(*flow, ss, format + INDENT));
  }
  write(ss, format + "}\n");
  // ss << format << "}\n";
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitActionDef(koda::ActionDef& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitRosDef(koda::RosDef& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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

  write(ss, ": ");
  begin(*node.def, ss);
  write(ss, node.def->typeName + " " + node.def->name);
  end(*node.def, ss);
  write(ss, "(");
  // ss << ": " << node.def->typeName << " " << node.def->name + "(";
  RETURN_ON_FAILURE(emitDefArguments(node.def->args, ss));
  write(ss, ");\n");
  // ss << ");\n";

  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitVarsBlock(koda::VarsBlock& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitVarDef(koda::VarDef& varDef, std::stringstream& ss, const std::string& format)
{
  begin(varDef, ss);
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
  end(varDef, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitFlow(koda::Flow& flow, std::stringstream& ss, const std::string& format)
{
  begin(flow, ss);
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
  end(flow, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitStrategy(koda::Strategy& strategy, std::stringstream& ss, const std::string& format)
{
  begin(strategy, ss);
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

  end(strategy, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitSequence(koda::Strategy::Seq& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitJoin(koda::Strategy::Join& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitWithin(koda::Strategy::Within& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  write(ss, "within " + node.seconds + std::string(" do ("));
  // ss << "within " << node.seconds << " do (";
  RETURN_ON_FAILURE(emitStrategy(*node.a, ss, format));
  write(ss, ") else (");
  // ss << ") else (";
  RETURN_ON_FAILURE(emitStrategy(*node.b, ss, format));
  write(ss, ")");
  // ss << ")";
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitRepeat(koda::Strategy::Repeat& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  write(ss, "repeat " + node.iterations + std::string(" ") + std::to_string(node.seconds));
  // ss << "repeat " << node.iterations << " " << node.seconds;
  write(ss, " (");
  // ss << " (";
  RETURN_ON_FAILURE(emitStrategy(*node.a, ss, format));
  write(ss, ")");
  // ss << ")";
  for (const auto& handler : node.handlers)
    RETURN_ON_FAILURE(emitHandler(*handler, ss, format));
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitEnd(koda::Strategy::End& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  Q_UNUSED(node);
  Q_UNUSED(format);

  write(ss, "end");
  // ss << "end";
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitContinue(koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  write(ss, "continue");
  // ss << "continue";
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitRef(koda::Strategy::Ref& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  write(ss, node.name);
  end(node, ss);
  // ss << node.name;
  return VoidResult();
}

VoidResult KodaEmitter::emitTaskCall(koda::Strategy::TaskCall& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  RETURN_ON_FAILURE(emitEventCall(*node.call, ss, format));
  for (const auto& handler : node.handlers)
    RETURN_ON_FAILURE(emitHandler(*handler, ss, format));
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitEventCall(koda::EventCall& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitHandler(koda::StrategyHandler& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
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
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitExpression(koda::Expr& node, std::stringstream& ss, const std::string& format)
{
  begin(node, ss);
  IF_ALT(PId, node.v, emitId, ss, format)
  ELSE_IF_ALT(PStr, node.v, emitStr, ss, format)
  ELSE_IF_ALT(PInt, node.v, emitInt, ss, format)
  ELSE_IF_ALT(PFloat, node.v, emitFloat, ss, format)
  ELSE_IF_ALT(PCall, node.v, emitCall, ss, format)
  ELSE_IF_ALT(PNeg, node.v, emitNeg, ss, format)
  ELSE_IF_ALT(PNot, node.v, emitNot, ss, format)
  ELSE_IF_ALT(PBinOp, node.v, emitBinOp, ss, format)
  ELSE_IF_ALT(PEParen, node.v, emitParen, ss, format)
  end(node, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitId(koda::Expr::Id& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  // ss << expr.value;
  write(ss, expr.value);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitStr(koda::Expr::Str& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  // ss << "\"" << expr.value << "\"";
  write(ss, "\"" + expr.value + "\"");
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitInt(koda::Expr::Int& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  // ss << expr.value;
  write(ss, "" + expr.value);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitFloat(koda::Expr::Float& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  // ss << expr.value;
  write(ss, std::to_string(expr.value));
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitCall(koda::Expr::Call& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitNeg(koda::Expr::Neg& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  std::stringstream ssOut("");
  ssOut << expr.value;
  std::string out;
  ssOut >> out;
  write(ss, out);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitNot(koda::Expr::Not& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitBinOp(koda::Expr::BinOp& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitParen(koda::Expr::Paren& expr, std::stringstream& ss, const std::string& format)
{
  begin(expr, ss);
  end(expr, ss);
  return VoidResult();
}

VoidResult KodaEmitter::emitCallArguments(std::vector<std::shared_ptr<Expr>>& args, std::stringstream& ss)
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

VoidResult KodaEmitter::emitDefArguments(std::vector<std::shared_ptr<Argument>>& args, std::stringstream& ss)
{
  for (size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      // ss << ", ";
      write(ss, ", ");

    const auto& arg = args[i];
    begin(*arg, ss);
    if (arg->kind == koda::Argument::Kind::Req)
      write(ss, arg->a + " req " + arg->b);
      // ss << arg->a << " req " << arg->b;
    else
      write(ss, arg->a + " " + arg->b);
      // ss << arg->a << " " << arg->b;
    end(*arg, ss);
  }

  return VoidResult();
}

}  // namespace koda
