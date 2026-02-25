#include "ast.h"

#include "logging.h"
#include "string_helpers.h"

#define IF_ALT(ALT, OBJ, CALL)          \
  if (std::holds_alternative<ALT>(OBJ)) \
  {                                     \
    if (auto obj = std::get<ALT>(OBJ))  \
      obj->CALL;                        \
  }

#define ELSE_IF_ALT(ALT, OBJ, CALL)          \
  else if (std::holds_alternative<ALT>(OBJ)) \
  {                                          \
    if (auto obj = std::get<ALT>(OBJ))       \
      obj->CALL;                             \
  }

#define LOG_TREE(TITLE)        \
  LOG_INFO("%s%s %s%s:",       \
           prefix.c_str(),     \
           tree::branch(last), \
           TITLE,              \
           gPrintSpan ? span.toString().c_str() : "");

namespace tree
{
inline const char* branch(bool last)
{
  return last ? " └─" : " ├─";
}
inline const char* carry(bool last)
{
  return last ? "   " : " │ ";
}
}  // namespace tree

namespace koda
{

bool gPrintSpan = true;

std::string Span::toString() const
{
  return " [(" + std::to_string(lineStart) + ", " + std::to_string(colStart) + ") - " +
         "(" + std::to_string(lineEnd) + ", " + std::to_string(colEnd) + ")]";
}

// TODO: Implement rest of the prints
// =============================================================
// Print
// =============================================================
void System::print() const
{
  LOG_INFO("  System");
  for (uint32_t i = 0; i < components.size(); ++i)
    components.at(i)->print(" ", i == components.size() - 1);
}

std::string Component::toString() const
{
  if (kind == Component::Kind::Task)
    return "Task";
  else if (kind == Component::Kind::Capability)
    return "Capability";
  else
    return "Unknown";
}

void printString(const std::string& prefix, const bool last, const std::string& message)
{
  LOG_INFO("%s%s %s", prefix.c_str(), tree::branch(last), message.c_str());
}

void Component::print(const std::string& prefix, const bool last) const
{
  LOG_TREE(toString().c_str());

  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Name: %s", name.c_str()));

  for (uint32_t i = 0; i < args.size(); ++i)
    args.at(i)->print(childPrefix, (i == args.size() - 1) && (statements.empty()));

  for (uint32_t i = 0; i < statements.size(); ++i)
    statements.at(i)->print(childPrefix, i == statements.size() - 1);
}

std::string Argument::toString() const
{
  if (kind == Argument::Kind::Plain)
    return "plain";
  else if (kind == Argument::Kind::Req)
    return "req";
  else if (kind == Argument::Kind::Pro)
    return "pro";
  else
    return "Unknown";
}

void Argument::print(const std::string& prefix, const bool last) const
{
  LOG_TREE(Format("Arg %s", toString().c_str()).c_str());

  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Type: %s", a.c_str()));
  printString(childPrefix, true, Format("Id: %s", b.c_str()));
}

// ---------- Statements ----------
void Statement::print(const std::string& prefix, const bool last) const
{
  if (std::holds_alternative<PStrategyBlock>(node))
    std::get<PStrategyBlock>(node)->print(prefix, last);
  else if (std::holds_alternative<PVarsBlock>(node))
    std::get<PVarsBlock>(node)->print(prefix, last);
  else if (std::holds_alternative<PRosDef>(node))
    std::get<PRosDef>(node)->print(prefix, last);
  else if (std::holds_alternative<PActionDef>(node))
    std::get<PActionDef>(node)->print(prefix, last);
}

void StrategyBlock::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("StrategyBlock");

  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < flows.size(); ++i)
    flows.at(i)->print(childPrefix, i == flows.size() - 1);
}

void VarsBlock::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("VarsBlock");

  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < vars.size(); ++i)
    vars.at(i)->print(childPrefix, i == vars.size() - 1);
}

std::string RosDef::toString() const
{
  if (kind == RosDef::Kind::Trigger)
    return "Trigger";
  else if (kind == RosDef::Kind::Return)
    return "Return";
  else if (kind == RosDef::Kind::Abort)
    return "Abort";
  else if (kind == RosDef::Kind::Error)
    return "Error";
  else if (kind == RosDef::Kind::In)
    return "In";
  else if (kind == RosDef::Kind::Out)
    return "Out";
  else
    return "Unknown";
}

void RosDef::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("RosDef");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Type: %s", toString().c_str()));

  def->print(childPrefix, true);
}

std::string ActionDef::toString() const
{
  if (kind == ActionDef::Kind::Action)
    return "Action";
  else if (kind == ActionDef::Kind::Service)
    return "Service";
  else if (kind == ActionDef::Kind::Topic)
    return "Topic";
  else
    return "Unknown";
}

void ActionDef::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("ActionDef");

  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Type: %s", toString().c_str()));
  printString(childPrefix, false, Format("Channel: %s", label1.c_str()));
  printString(childPrefix, rosDefs.empty(), Format("Message: %s", label2.c_str()));

  for (uint32_t i = 0; i < rosDefs.size(); ++i)
    rosDefs.at(i)->print(childPrefix, i == rosDefs.size() - 1);
}

void VarDef::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("VarDef");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Type: %s", varType.c_str()));
  printString(childPrefix, false, Format("Name: %s", name.c_str()));
  if (init)
    init->print(childPrefix, false);
  if (fallback)
    fallback->print(childPrefix, true);
}

void EventDef::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("EventDef");

  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Type: %s", typeName.c_str()));
  printString(childPrefix, args.empty() && components.empty(), Format("Name: %s", name.c_str()));
  for (uint32_t i = 0; i < args.size(); ++i)
    args.at(i)->print(childPrefix, (i == args.size() - 1) && components.empty());
  for (uint32_t i = 0; i < components.size(); ++i)
    components.at(i)->print(childPrefix, i == components.size() - 1);
}

void EventDefComponent::print(const std::string& prefix, const bool last) const
{
  LOG_INFO("%s%s%s %s", prefix.c_str(), tree::branch(last), kind.c_str(), text.c_str());
}

// -------------------------------------------------------------
// Flow
void Flow::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("Flow")
  const std::string childPrefix = prefix + tree::carry(last);
  std::string tagsString = "";
  for (uint32_t i = 0; i < tags.size(); ++i)
    tagsString += tagsString + (i < tags.size() - 1 ? ", " : "");

  printString(childPrefix, false, Format("Name: %s", name.c_str()));
  if (!tagsString.empty())
    printString(childPrefix, false, Format("Tags: %s", tagsString.c_str()));

  strategy->print(childPrefix, true);
}

void Strategy::print(const std::string& prefix, const bool last) const
{
  IF_ALT(PSeq, v, print(prefix, last, span))
  ELSE_IF_ALT(PJoin, v, print(prefix, last, span))
  ELSE_IF_ALT(PEither, v, print(prefix, last, span))
  ELSE_IF_ALT(PLet, v, print(prefix, last, span))
  ELSE_IF_ALT(PWithin, v, print(prefix, last, span))
  ELSE_IF_ALT(PIfElse, v, print(prefix, last, span))
  ELSE_IF_ALT(PRepeat, v, print(prefix, last, span))
  ELSE_IF_ALT(PGuard, v, print(prefix, last, span))
  ELSE_IF_ALT(PEvery, v, print(prefix, last, span))
  ELSE_IF_ALT(PEnd, v, print(prefix, last, span))
  ELSE_IF_ALT(PRef, v, print(prefix, last, span))
  ELSE_IF_ALT(PTaskCall, v, print(prefix, last, span))
  ELSE_IF_ALT(PParen, v, print(prefix, last, span))
}

void Strategy::Seq::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Seq");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a != nullptr)
    a->print(childPrefix, false);
  if (b != nullptr)
    b->print(childPrefix, true);
}

void Strategy::Join::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Join");
  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < alts.size(); ++i)
    alts.at(i)->print(childPrefix, i == alts.size() - 1);
}

void Strategy::Either::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Either");
  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < alts.size(); ++i)
    alts.at(i)->print(childPrefix, i == alts.size() - 1);
}

void Strategy::Let::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Let");
}

void Strategy::Within::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Within");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a != nullptr)
    a->print(childPrefix, false);
  if (b != nullptr)
    b->print(childPrefix, true);
}

void Strategy::IfElse::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("IfElse");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a != nullptr)
    a->print(childPrefix, false);
  if (b != nullptr)
    b->print(childPrefix, true);
}

void Strategy::Repeat::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Repeat");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a)
    a->print(childPrefix, true);
}

void Strategy::Guard::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Guard");
}

void Strategy::Every::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Every");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a)
    a->print(childPrefix, handlers.empty());

  for (uint32_t i = 0; i < handlers.size(); ++i)
    handlers.at(i)->print(childPrefix, i == handlers.size() - 1);
}

void Strategy::End::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("End");
}

void Strategy::Ref::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Reference");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Ref: %s", name.c_str()));
}

void Strategy::TaskCall::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("TaskCall");
  const std::string childPrefix = prefix + tree::carry(last);
  if (call)
    call->print(childPrefix, handlers.empty());

  for (uint32_t i = 0; i < handlers.size(); ++i)
    handlers.at(i)->print(childPrefix, i == handlers.size() - 1);
}

void Strategy::Paren::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Paren");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a)
    a->print(childPrefix, true);
}

std::string StrategyHandler::toString() const
{
  if (kind == Kind::OnError)
    return "On Error";
  else if (kind == Kind::OnAbort)
    return "On Abort";
  else if (kind == Kind::OnEmitter)
    return "On Emitter";
  else
    return "Unknown";
}

void StrategyHandler::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("Interrupt handler");

  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Type: %s", toString().c_str()));
  if (emitter)
    emitter->print(childPrefix, false);
  if (body)
    body->print(childPrefix, true);
}

// -------------------------------------------------------------
// Event call
void EventCall::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("EventCall");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, receiver.empty() && args.empty(), Format("Name: %s", name.c_str()));

  if (!receiver.empty())
    printString(childPrefix, args.empty(), Format("Receiver: %s", receiver.c_str()));

  if (!args.empty())
  {
    printString(childPrefix, true, Format("Args:"));
    const std::string grandChildPrefix = childPrefix + tree::carry(true);
    for (uint32_t i = 0; i < args.size(); ++i)
      args.at(i)->print(grandChildPrefix, i == args.size() - 1);
  }
}

// -------------------------------------------------------------
// Expressions
void Expr::print(const std::string& prefix, const bool last) const
{
  IF_ALT(PId, v, print(prefix, last, span))
  ELSE_IF_ALT(PStr, v, print(prefix, last, span))
  ELSE_IF_ALT(PInt, v, print(prefix, last, span))
  ELSE_IF_ALT(PFloat, v, print(prefix, last, span))
  ELSE_IF_ALT(PCall, v, print(prefix, last, span))
  ELSE_IF_ALT(PNeg, v, print(prefix, last, span))
  ELSE_IF_ALT(PNot, v, print(prefix, last, span))
  ELSE_IF_ALT(PBinOp, v, print(prefix, last, span))
  ELSE_IF_ALT(PEParen, v, print(prefix, last, span))
}

void Expr::Id::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Id");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %s", value.c_str()));
}

void Expr::Str::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("String");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %s", value.c_str()));
}

void Expr::Int::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Integer");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %d", value));
}

void Expr::Float::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Float");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %.6lf", value));
}

void Expr::Call::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Call");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

void Expr::Neg::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Unary");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

void Expr::Not::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Negation");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

std::string Expr::BinOp::toString() const
{
  if (operation == Expr::BinOp::Kind::Equal)
    return "Equal";
  else if (operation == Expr::BinOp::Kind::NotEqual)
    return "NotEqual";
  else if (operation == Expr::BinOp::Kind::GreaterThan)
    return "GreaterThan";
  else if (operation == Expr::BinOp::Kind::GreaterEqual)
    return "GreaterEqual";
  else if (operation == Expr::BinOp::Kind::LessThan)
    return "LessThan";
  else if (operation == Expr::BinOp::Kind::LessEqual)
    return "LessEqual";
  else
    return "Unknown";
}

void Expr::BinOp::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("BinOp");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, false, Format("Operation: %s", toString().c_str()));
  if (a)
    a->print(childPrefix, false);
  if (b)
    b->print(childPrefix, true);
}

void Expr::Paren::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Paren");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

// =============================================================
}  // namespace koda