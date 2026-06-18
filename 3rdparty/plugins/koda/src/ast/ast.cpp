#include "ast.h"

#include "../maki_to_koda.h"
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

#define LOG_TREE(TITLE)       \
  LOG_RAW("{}{} {}{}:",       \
          prefix,             \
          tree::branch(last), \
          TITLE,              \
          gPrintSpan ? span.toString() : "");

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

QJsonObject Span::toJson() const
{
    QJsonObject obj;
    obj["lineStart"] = lineStart;
    obj["colStart"] = colStart;
    obj["lineEnd"] = lineEnd;
    obj["colEnd"] = colEnd;
    return obj;
}

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
  LOG_RAW("  System");
  for (uint32_t i = 0; i < components.size(); ++i)
    components.at(i)->print(" ", i == components.size() - 1);
}

QJsonObject System::toJson() const {
  QJsonObject obj;
  QJsonArray compArray;
  for (const auto& component : components)
  {
    if (component)
        compArray.append(component->toJson());
  }
  return obj;
}

std::string Component::kindToString() const
{
    switch (kind)
    {
        case Kind::Task:
            return "Task";
        case Kind::Capability:
            return "Capability";
        default:
            return "Unknown";
    }
}

QJsonObject Component::toJson() const
{
    QJsonObject obj;

    // basic fields
    obj["kind"] = QString::fromStdString(kindToString());
    obj["name"] = QString::fromStdString(name);
    obj["srcId"] = QString::fromStdString(srcId);

    // span
    obj["span"] = span.toJson();

    // args array
    QJsonArray argsArray;
    for (const auto& arg : args)
    {
        if (arg)
            argsArray.append(arg->toJson());
    }
    obj["args"] = argsArray;

    // statements array
    QJsonArray stmtArray;
    for (const auto& stmt : statements)
    {
        if (stmt)
            stmtArray.append(stmt->toJson());
    }
    obj["statements"] = stmtArray;

    return obj;
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
  LOG_RAW("{}{} {}", prefix.c_str(), tree::branch(last), message.c_str());
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

std::string Argument::aKindToString() const
{
  if (kind == Argument::Kind::Plain)
    return "Plain";
  else if (kind == Argument::Kind::Req)
    return "Req";
  else if (kind == Argument::Kind::Pro)
    return "Pro";
  else
    return "Unknown";
}

QJsonObject Argument::toJson() const
{
  QJsonObject obj;
  obj["kind"] = QString::fromStdString(aKindToString());
  obj["a"] = QString::fromStdString(a);
  obj["b"] = QString::fromStdString(b);
  obj["srcId"] = QString::fromStdString(srcId);
  obj["span"] = span.toJson();
  return obj;
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

QJsonObject Statement::toJson() const
{
  QJsonObject obj;
  if (node)
    obj["node"] = node.toJson();
  obj["span"] = span.toJson();
  return obj;
}

void StrategyBlock::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("StrategyBlock");

  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < flows.size(); ++i)
    flows.at(i)->print(childPrefix, i == flows.size() - 1);
}

QJsonObject StrategyBlock::toJson() const {
  QJsonObject obj;
  QJsonArray flowArray;
  for (const auto& flow : flows) {
    if (flow)
      flowArray.append(flow->toJson());
  }
  obj["flows"] = flowArray;
  obj["span"] = span.toJson();
  return obj;
}

void VarsBlock::print(const std::string& prefix, const bool last) const
{
  LOG_TREE("VarsBlock");

  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < vars.size(); ++i)
    vars.at(i)->print(childPrefix, i == vars.size() - 1);
}

QJsonObject VarsBlock::toJson() const {
  QJsonObject obj;
  QJsonArray varsArray;
  for (const auto& var : vars) {
    if (var)
      varsArray.append(var->toJson());
  }
  obj["vars"] = varsArray;
  obj["span"] = span.toJson();
  return obj;
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

std::string RosDef::rKindToString() const
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

QJsonObject RosDef::toJson() const {
  QJsonObject obj;
  obj["kind"] = QString::fromStdString(rKindToString());
  if (def)
    obj["def"] = def->toJson();
  obj["srcId"] = QString::fromStdString(srcId);
  obj["span"] = span.toJson();
  return obj;
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

std::string ActionDef::adKindToString() const {
  if (kind == ActionDef::Kind::Action)
    return "Action";
  else if (kind == ActionDef::Kind::Service)
    return "Service";
  else if (kind == ActionDef::Kind::Topic)
    return "Topic";
  else
    return "Unknown";
}

QJsonObject ActionDef::toJson() const {
  QJsonObject obj;
  obj["kind"] = QString::fromStdString(adKindToString());
  obj["label1"] = QString::fromStdString(label1);
  obj["label2"] = QString::fromStdString(label2);
  obj["srcId"] = QString::fromStdString(srcId);
  QJsonArray rosDefsArray;
  for (const auto& rosDef : rosDefs) {
    if (rosDef)
      rosDefsArray.append(rosDef->toJson());
  }
  obj["rosDefs"] = rosDefsArray;
  obj["span"] = span.toJson();
  return obj;
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

QJsonObject VarDef::toJson() const
{
  QJsonObject obj;
  obj["varType"] = QString::fromStdString(varType);
  obj["name"] = QString::fromStdString(name);
  obj["srcId"] = QString::fromStdString(srcId);
  if (init)
    obj["init"] = init->toJson();
  if (fallback)
    obj["fallback"] = fallback->toJson();
  obj["span"] = span.toJson();
  return obj;
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

QJsonObject EventDef::toJson() const {
  QJsonObject obj;
  obj["typeName"] = QString::fromStdString(typeName);
  obj["name"] = QString::fromStdString(name);
  obj["srcId"] = QString::fromStdString(srcId);
  QJsonArray argsArray;
  for (const auto& arg : args) {
    if (arg)
      argsArray.append(arg->toJson());
  }
  obj["args"] = argsArray;
  QJsonArray componentsArray;
  for (const auto& component : components) {
    if (component)
      componentsArray.append(component->toJson());
  }
  obj["components"] = componentsArray;
  obj["span"] = span.toJson();
  return obj;
}

void EventDefComponent::print(const std::string& prefix, const bool last) const
{
  LOG_RAW("{}{}{} {}", prefix.c_str(), tree::branch(last), kind.c_str(), text.c_str());
}

QJsonObject EventDef::toJson() const {
  QJsonObject obj;
  obj["kind"] = QString::fromStdString(kind);
  obj["text"] = QString::fromStdString(text);
  return obj;
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

QJsonObject Flow::toJson() const
{
  QJsonObject obj;
  obj["name"] = QString::fromStdString(name);
  obj["srcId"] = QString::fromStdString(srcId);
  QJsonArray tagsArray;
  for (const auto& tag : tags) {
    if (tag)
      tagsArray.append(QString::fromStdString(tag));
  }
  obj["tags"] = tagsArray;
  if (strategy)
    obj["strategy"] = strategy->toJson();
  obj["span"] = span.toJson();
  return obj;
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
  ELSE_IF_ALT(PEnd, v, print(prefix, last, span))
  ELSE_IF_ALT(PContinue, v, print(prefix, last, span))
  ELSE_IF_ALT(PRef, v, print(prefix, last, span))
  ELSE_IF_ALT(PTaskCall, v, print(prefix, last, span))
  ELSE_IF_ALT(PParen, v, print(prefix, last, span))
}

QJsonObject Strategy::toJson() const {
  QJsonObject obj;
  if (v)
    obj["v"] = v.toJson();
  obj["srcId"] = QString::fromStdString(srcId);
  obj["span"] = span.toJson();
  return obj;
}

void Strategy::Seq::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Seq");
  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < alts.size(); ++i)
    alts.at(i)->print(childPrefix, i == alts.size() - 1);
}

QJsonObject Strategy::Seq::toJson() const
{
  QJsonObject obj;
  QJsonArray altsArray;
  for (const auto& alt: alts) {
    if (alt)
      altsArray.append(alt->toJson());
  }
  obj["alts"] = altsArray;
  return obj;
}

void Strategy::Join::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Join");
  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < alts.size(); ++i)
    alts.at(i)->print(childPrefix, i == alts.size() - 1);
}
QJsonObject Strategy::Join::toJson() const {
  QJsonObject obj;
  QJsonArray altsArray;
  for (const auto& alt: alts) {
    if (alt)
      altsArray.append(alt->toJson());
  }
  obj["alts"] = altsArray;
  return obj;
}

void Strategy::Either::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Either");
  const std::string childPrefix = prefix + tree::carry(last);
  for (uint32_t i = 0; i < alts.size(); ++i)
    alts.at(i)->print(childPrefix, i == alts.size() - 1);
}

QJsonObject Strategy::Either::toJson() const {
  QJsonObject obj;
  QJsonArray altsArray;
  for (const auto& alt: alts) {
    if (alt)
      altsArray.append(alt->toJson());
  }
  obj["alts"] = altsArray;
  return obj;
}

void Strategy::Let::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Let");
}

QJsonObject Strategy::Let::toJson() const {
  QJsonObject obj;
  obj["name"] = QString::fromStdString(name);
  if (call)
    obj["call"] = call->toJson();
  return obj;
}

void Strategy::Within::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Within");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a != nullptr)
    a->print(childPrefix, false);
  if (b != nullptr)
    b->print(childPrefix, handlers.empty());
  for (int i = 0; i < handlers.size(); ++i)
    handlers.at(i)->print(childPrefix, i + 1 == handlers.size());
}

QJsonObject Strategy::Within::toJson() const {
  QJsonObject obj;
  obj["seconds"] = seconds;
  if (a)
    obj["a"] = a->toJson();
  if (b)
    obj["b"] = b->toJson();
  QJsonArray handlersArray;
  for (const auto& handler: handlers) {
    if (handler)
      handlersArray.append(handler->toJson());
  }
  obj["handlers"] = handlersArray;
  return obj;
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

QJsonObject Strategy::IfElse::toJson() const {
  QJsonObject obj;
  if (cond)
    obj["cond"] = cond->toJson();
  if (a)
    obj["a"] = a->toJson();
  if (b)
    obj["b"] = b->toJson();
  return obj;
}

void Strategy::Repeat::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Repeat");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a)
    a->print(childPrefix, true);
}

QJsonObject Strategy::Repeat::toJson() const {
  QJsonObject obj;
  obj["seconds"] = seconds;
  obj["iterations"] = iterations;
  if (a)
    obj["a"] = a->toJson();
  QJsonArray handlersArray;
  for (const auto& handler: handlers) {
    if (handler)
      handlersArray.append(handler->toJson());
  }
  obj["handlers"] = handlersArray;
  return obj;
}

void Strategy::Guard::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Guard");
}

QJsonObject Strategy::Guard::toJson() const {
  QJsonObject obj;
  if (cond)
    obj["cond"] = cond->toJson();
  return obj;
}

void Strategy::End::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("End");
}

QJsonObject Strategy::End::toJson() const {
  QJsonObject obj;
  return obj;
}

void Strategy::Continue::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Continue");
}

QJsonObject Strategy::Continue::toJson() const {
  QJsonObject obj;
  return obj;
}

void Strategy::Ref::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Reference");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Ref: %s", name.c_str()));
}

QJsonObject Strategy::Ref::toJson() const {
  QJsonObject obj;
  obj["name"] = QString::fromStdString(name);
  return obj;
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

QJsonObject Strategy::TaskCall::toJson() const {
  QJsonObject obj;
  if (call)
    obj["call"] = call->toJson();
  QJsonArray handlersArray;
  for (const auto& handler: handlers) {
    if (handler)
      handlersArray.append(handler->toJson());
  }
  obj["handlers"] = handlersArray;
  return obj;
}

void Strategy::Paren::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Paren");
  const std::string childPrefix = prefix + tree::carry(last);
  if (a)
    a->print(childPrefix, true);
}

QJsonObject Strategy::Paren::toJson() const {
  QJsonObject obj;
  if (a)
    obj["a"] = a->toJson();
  return obj;
}

std::string StrategyHandler::toString() const
{
  if (kind == Kind::OnError)
    return "On Error";
  else if (kind == Kind::OnAbort)
    return "On Abort";
  else if (kind == Kind::OnEmitter)
    return "On Emitter";
  else if (kind == Kind::OnEmitterContinue)
    return "On Emitter Continue";
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

QJsonObject StrategyHandler::toJson() const {
  QJsonObject obj;
  obj["kind"] = QString::fromStdString(toString());
  if (emitter)
    obj["emitter"] = emitter->toJson();
  if (body)
    obj["body"] = body->toJson();
  obj["srcId"] = QString::fromStdString(srcId);
  obj["span"] = span.toJson();
  return obj;
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

QJsonObject EventCall::toJson() const {
  QJsonObject obj;
  obj["name"] = QString::fromStdString(name);
  obj["receiver"] = QString::fromStdString(receiver);
  obj["srcId"] = QString::fromStdString(srcId);
  QJsonArray argsArray;
  for (const auto& arg: args) {
    if (arg)
      argsArray.append(arg->toJson());
  }
  obj["args"] = argsArray;
  obj["span"] = span.toJson();
  return obj;
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

QJsonObject Expr::toJson() const {
  QJsonObject obj;
  if (v)
    obj["v"] = v->toJson();
  obj["srcId"] = QString::fromStdString(srcId);
  obj["span"] = span.toJson();
  return obj;
}

void Expr::Id::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Id");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %s", value.c_str()));
}

QJsonObject Expr::Id::toJson() const {
  QJsonObject obj;
  obj["value"] = QString::fromStdString(value);
  return obj;
}

void Expr::Str::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("String");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %s", value.c_str()));
}

QJsonObject Expr::Str::toJson() const {
  QJsonObject obj;
  obj["value"] = QString::fromStdString(value);
  return obj;
}

void Expr::Int::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Integer");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %d", value));
}

QJsonObject Expr::Int::toJson() const {
  QJsonObject obj;
  obj["value"] = value;
  return obj;
}

void Expr::Float::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Float");
  const std::string childPrefix = prefix + tree::carry(last);
  printString(childPrefix, true, Format("Value: %.6lf", value));
}

QJsonObject Expr::Float::toJson() const {
  QJsonObject obj;
  obj["value"] = value;
  return obj;
}

void Expr::Call::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Call");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

QJsonObject Expr::Call::toJson() const {
  QJsonObject obj;
  if (value)
    obj["value"] = value->toJson();
  return obj;
}

void Expr::Neg::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Unary");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

QJsonObject Expr::Neg::toJson() const {
  QJsonObject obj;
  if (value)
    obj["value"] = value->toJson();
  return obj;
}

void Expr::Not::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Negation");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

QJsonObject Expr::Not::toJson() const {
  QJsonObject obj;
  if (value)
    obj["value"] = value->toJson();
  return obj;
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

QJsonObject Expr::BinOp::toJson() const {
  QJsonObject obj;
  obj["operation"] = QString::fromStdString(toString());
  if (a)
    obj["a"] = a->toJson();
  if (b)
    obj["b"] = b->toJson();
  return obj;
}

void Expr::Paren::print(const std::string& prefix, const bool last, const Span& span) const
{
  LOG_TREE("Paren");
  const std::string childPrefix = prefix + tree::carry(last);
  if (value)
    value->print(childPrefix, true);
}

QJsonObject Expr::Paren::toJson() const {
  QJsonObject obj;
  if (value)
    obj["value"] = value->toJson();
  return obj;
}

// =============================================================
}  // namespace koda
