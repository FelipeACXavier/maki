#include "koda_emitter.h"

#include <sstream>

#include "ast/ast.h"
#include "logging.h"
#include "result.h"
#include "types.h"

static constexpr std::string INDENT = "  ";

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
KodaEmitter::KodaEmitter(const koda::types::TypeRegistry* registry)
    : mTypeRegistry(registry)
{
}

Result<std::string> KodaEmitter::emitKoda(const koda::System& ast, const koda::types::TypeRegistry* registry)
{
  KodaEmitter emitter(registry);

  std::stringstream ss;
  RETURN_ON_FAILURE_AS(emitter.emitTypes(ss), std::string);

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

VoidResult KodaEmitter::emitTypes(std::stringstream& ss)
{
  std::set<std::string> emitted;
  std::set<std::string> visiting;

  for (const auto& definition : mTypeRegistry->allTypes())
  {
    if (!definition || definition->isPrimitive())
      continue;

    RETURN_ON_FAILURE(emitTypeRecursive(*definition, ss, emitted, visiting));
  }

  return VoidResult();
}

VoidResult KodaEmitter::emitType(const koda::types::TypeDefinition& definition, std::stringstream& ss, const std::string& format)
{
  if (definition.isPrimitive())
  {
    // Built-ins don't need declarations.
    return VoidResult();
  }

  if (definition.isAlias())
  {
    const auto alias = definition.alias();

    auto target = emitTypeReference(alias.target);
    RETURN_ON_FAILURE(target);

    ss << format << "type " << definition.name.toString() << " = " << target.Value() << ";\n";

    return VoidResult();
  }

  if (definition.isEnum())
  {
    const auto enumeration = definition.enumeration();

    ss << format << std::format("enum {} : {} {{\n", definition.name.toString(), koda::types::toString(enumeration.underlyingType));

    for (const auto& value : enumeration.values)
    {
      ss << format << INDENT << value.name;

      if (value.value)
        ss << " = " << value.value.value();

      ss << ";\n";
    }

    ss << format << "}\n";

    return VoidResult();
  }

  if (definition.isRecord())
  {
    const auto record = definition.record();

    ss << format << "type " << definition.name.toString();

    if (record.baseType)
    {
      const auto& baseReference = record.baseType.value();

      const auto* baseDefinition = mTypeRegistry->resolve(baseReference);

      if (!baseDefinition)
        return VoidResult::Failed("Could not find base type: {}", baseReference.toString());

      if (!baseDefinition->isRecord())
        return VoidResult::Failed("Base type '{}' of record '{}' is not a record", baseReference.toString(), definition.name.toString());

      auto baseType = emitTypeReference(baseReference);
      RETURN_ON_FAILURE(baseType);

      ss << " extends " << baseType.Value();
    }

    ss << " {\n";

    for (const auto& field : record.fields)
    {
      auto fieldType = emitTypeReference(field.type);
      RETURN_ON_FAILURE(fieldType);

      ss << format << INDENT << field.name << ": " << fieldType.Value() << ";\n";
    }

    ss << format << "}\n";

    return VoidResult();
  }

  return VoidResult::Failed("Unsupported type definition: {}", definition.name.toString());
}

VoidResult KodaEmitter::emitTypeRecursive(const koda::types::TypeDefinition& definition, std::stringstream& ss, std::set<std::string>& emitted,
                                          std::set<std::string>& visiting)
{
  const auto name = definition.name.toString();

  if (emitted.contains(name))
    return VoidResult();

  if (visiting.contains(name))
    return VoidResult::Failed("Recursive type dependency detected involving '{}'", name);

  visiting.insert(name);

  for (const auto& dependency : typeDependencies(definition))
  {
    const auto* dependencyDefinition = mTypeRegistry->resolve(dependency);

    if (!dependencyDefinition)
      return VoidResult::Failed("Could not resolve dependency '{}' of type '{}'", dependency.toString(), name);

    if (!dependencyDefinition->isPrimitive())
      RETURN_ON_FAILURE(emitTypeRecursive(*dependencyDefinition, ss, emitted, visiting));
  }

  visiting.erase(name);

  RETURN_ON_FAILURE(emitType(definition, ss, ""));

  emitted.insert(name);

  return VoidResult();
}

Result<std::string> KodaEmitter::emitTypeReference(const koda::types::TypeReference& reference) const
{
  if (!reference.isValid())
    return Result<std::string>::Failed("Invalid type reference");

  switch (reference.kind())
  {
    case koda::types::TypeReferenceKind::List:
    {
      auto element = emitTypeReference(reference.elementType());
      RETURN_ON_FAILURE_AS(element, std::string);

      return std::format("list<{}>", element.Value());
    }

    case koda::types::TypeReferenceKind::Map:
    {
      auto key = emitTypeReference(reference.mapKeyType());
      RETURN_ON_FAILURE_AS(key, std::string);

      auto value = emitTypeReference(reference.mapValueType());
      RETURN_ON_FAILURE_AS(value, std::string);

      return std::format("map<{}, {}>", key.Value(), value.Value());
    }

    default:
      // Primitive and named references can already render themselves.
      return reference.toString();
  }
}

std::vector<koda::types::TypeReference> KodaEmitter::typeDependencies(const koda::types::TypeDefinition& definition) const
{
  std::vector<koda::types::TypeReference> out;

  if (definition.isAlias())
  {
    collectTypeDependencies(definition.alias().target, out);
  }
  else if (definition.isRecord())
  {
    const auto& record = definition.record();

    if (record.baseType)
      collectTypeDependencies(record.baseType.value(), out);

    for (const auto& field : record.fields)
      collectTypeDependencies(field.type, out);
  }

  return out;
}

void KodaEmitter::collectTypeDependencies(const koda::types::TypeReference& reference, std::vector<koda::types::TypeReference>& out) const
{
  switch (reference.kind())
  {
    case koda::types::TypeReferenceKind::List:
      collectTypeDependencies(reference.elementType(), out);
      return;

    case koda::types::TypeReferenceKind::Map:
      collectTypeDependencies(reference.mapKeyType(), out);

      collectTypeDependencies(reference.mapValueType(), out);
      return;

    case koda::types::TypeReferenceKind::Named:
      out.push_back(reference);
      return;

    case koda::types::TypeReferenceKind::Primitive:
      return;

    case koda::types::TypeReferenceKind::Optional:
    case koda::types::TypeReferenceKind::Unknown:
      LOG_WARNING("Unsuported type for: {}", reference.toString());
      return;
  }
}

VoidResult KodaEmitter::emitTask(const koda::Component& component, std::stringstream& ss)
{
  ss << "task " << component.name << "(";
  RETURN_ON_FAILURE(emitDefArguments(component.args, ss));
  ss << ") {\n";
  for (const auto& actions : component.statements)
    RETURN_ON_FAILURE(emitStatement(*actions, ss, INDENT));
  ss << "}\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitCapability(const koda::Component& component, std::stringstream& ss)
{
  ss << "capability " << component.name << "(";
  RETURN_ON_FAILURE(emitDefArguments(component.args, ss));
  ss << ") {\n";
  for (const auto& actions : component.statements)
    RETURN_ON_FAILURE(emitStatement(*actions, ss, INDENT));
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
    RETURN_ON_FAILURE(emitFlow(*flow, ss, format + INDENT));
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

  ss << format << "parameters {\n";

  for (const auto& var : node.vars)
    RETURN_ON_FAILURE(emitVarDef(*var, ss, format + INDENT));

  ss << format << "}\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitVarDef(const koda::VarDef& varDef, std::stringstream& ss, const std::string& format)
{
  auto type = emitTypeReference(varDef.varType);
  RETURN_ON_FAILURE(type);

  ss << format << type.Value() << " " << varDef.name << " = ";

  RETURN_ON_FAILURE(emitExpression(*varDef.init, ss, format));

  ss << ";\n";

  return VoidResult();
}

VoidResult KodaEmitter::emitFlow(const koda::Flow& flow, std::stringstream& ss, const std::string& format)
{
  ss << format << flow.name;
  if (!flow.args.empty())
  {
    ss << "[";
    bool first = true;
    for (const auto& arg : flow.args)
    {
      if (!first)
        ss << ", ";
      ss << arg->b;
      first = false;
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
  ELSE_IF_ALT(PTaskCall, strategy.v, emitTaskCall, ss, format)
  // ELSE_IF_ALT(PParen, strategy.v, emitParen, ss, format)
  // ELSE_IF_ALT(PEither, strategy.v, emitEither, ss, format)

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
  ss << "end";
  return VoidResult();
}

VoidResult KodaEmitter::emitContinue(const koda::Strategy::Continue& node, std::stringstream& ss, const std::string& format)
{
  ss << "continue";
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
  // Async call
  if (node.receiver.empty())
    ss << node.name;
  else
    ss << node.receiver << "." << node.name;

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
  ELSE_IF_ALT(PBool, node.v, emitBool, ss, format)
  ELSE_IF_ALT(PCall, node.v, emitCall, ss, format)
  ELSE_IF_ALT(PNeg, node.v, emitNeg, ss, format)
  ELSE_IF_ALT(PNot, node.v, emitNot, ss, format)
  ELSE_IF_ALT(PBinOp, node.v, emitBinOp, ss, format)
  ELSE_IF_ALT(PEParen, node.v, emitParen, ss, format)
  ELSE_IF_ALT(PRecordLiteral, node.v, emitRecordLiteral, ss, format)
  ELSE_IF_ALT(PListLiteral, node.v, emitListLiteral, ss, format)
  ELSE_IF_ALT(PMapLiteral, node.v, emitMapLiteral, ss, format)

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
  ss << std::fixed << std::setprecision(6) << expr.value;
  return VoidResult();
}

VoidResult KodaEmitter::emitBool(const koda::Expr::Bool& expr, std::stringstream& ss, const std::string& format)
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

VoidResult KodaEmitter::emitRecordLiteral(const koda::Expr::RecordLiteral& expr, std::stringstream& ss, const std::string& format)
{
  if (expr.fields.empty())
  {
    ss << "{}";
    return VoidResult();
  }

  ss << "{\n";
  size_t count = 0;
  for (const auto& field : expr.fields)
  {
    ss << format << INDENT << field->name << ": ";
    RETURN_ON_FAILURE(emitExpression(*field->value, ss, format + INDENT));
    if (count < expr.fields.size() - 1)
      ss << ",";

    ss << "\n";
    ++count;
  }
  ss << format << "}";

  return VoidResult();
}

VoidResult KodaEmitter::emitListLiteral(const koda::Expr::ListLiteral& expr, std::stringstream& ss, const std::string& format)
{
  if (expr.fields.empty())
  {
    ss << "[]";
    return VoidResult();
  }

  ss << "[\n";
  size_t count = 0;
  for (const auto& field : expr.fields)
  {
    ss << format << INDENT;
    RETURN_ON_FAILURE(emitExpression(*field, ss, format + INDENT));
    if (count < expr.fields.size() - 1)
      ss << ",";
    ss << "\n";
    ++count;
  }
  ss << format << "]";

  return VoidResult();
}

VoidResult KodaEmitter::emitMapLiteral(const koda::Expr::MapLiteral& expr, std::stringstream& ss, const std::string& format)
{
  if (expr.fields.empty())
  {
    ss << "{}";
    return VoidResult();
  }

  ss << "{\n";
  size_t count = 0;
  for (const auto& field : expr.fields)
  {
    ss << format << INDENT << "<";
    RETURN_ON_FAILURE(emitExpression(*field->key, ss, format + INDENT));
    ss << ", ";
    RETURN_ON_FAILURE(emitExpression(*field->value, ss, ""));
    ss << ">";
    if (count < expr.fields.size() - 1)
      ss << ",";
    ss << "\n";
    ++count;
  }
  ss << format << "}";

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
      ss << arg->b << " req " << arg->a.toString();
    else
      ss << arg->a.toString() << " " << arg->b;
  }

  return VoidResult();
}

}  // namespace koda