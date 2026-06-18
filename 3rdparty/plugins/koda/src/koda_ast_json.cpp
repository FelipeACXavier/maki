#include "koda_ast_json.h"

namespace koda
{
  QJsonObject System::toJson() {
    QJsonObject obj;
    QJsonArray compArray;
    for (const auto& component : components)
    {
      if (component)
        compArray.append(component->toJson());
    }
    obj["components"] = compArray;
    return obj;
  }

  QJsonObject Span::toJson()
  {
    QJsonObject obj;
    obj["lineStart"] = lineStart;
    obj["colStart"] = colStart;
    obj["lineEnd"] = lineEnd;
    obj["colEnd"] = colEnd;
    return obj;
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

  QJsonObject Statement::toJson() const
  {
    QJsonObject obj;
    if (node)
      obj["node"] = node.toJson();
    obj["span"] = span.toJson();
    return obj;
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

  QJsonObject EventDefComponent::toJson() const {
    QJsonObject obj;
    obj["kind"] = QString::fromStdString(kind);
    obj["text"] = QString::fromStdString(text);
    return obj;
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

  QJsonObject RosDef::toJson() const {
    QJsonObject obj;
    obj["kind"] = QString::fromStdString(rKindToString());
    if (def)
      obj["def"] = def->toJson();
    obj["srcId"] = QString::fromStdString(srcId);
    obj["span"] = span.toJson();
    return obj;
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

  QJsonObject Strategy::toJson() const {
    QJsonObject obj;
    if (v)
      obj["v"] = v.toJson();
    obj["srcId"] = QString::fromStdString(srcId);
    obj["span"] = span.toJson();
    return obj;
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

  QJsonObject Expr::toJson() const {
    QJsonObject obj;
    if (v)
      obj["v"] = v->toJson();
    obj["srcId"] = QString::fromStdString(srcId);
    obj["span"] = span.toJson();
    return obj;
  }

  // Strategy sub-structs
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

  QJsonObject Strategy::Let::toJson() const {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(name);
    if (call)
      obj["call"] = call->toJson();
    return obj;
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

  QJsonObject Strategy::Guard::toJson() const {
    QJsonObject obj;
    if (cond)
      obj["cond"] = cond->toJson();
    return obj;
  }

  QJsonObject Strategy::End::toJson() const {
    QJsonObject obj;
    return obj;
  }

  QJsonObject Strategy::Continue::toJson() const {
    QJsonObject obj;
    return obj;
  }

  QJsonObject Strategy::Ref::toJson() const {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(name);
    return obj;
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

  QJsonObject Strategy::Paren::toJson() const {
    QJsonObject obj;
    if (a)
      obj["a"] = a->toJson();
    return obj;
  }

  QJsonObject Expr::Id::toJson() const {
    QJsonObject obj;
    obj["value"] = QString::fromStdString(value);
    return obj;
  }

  QJsonObject Expr::Str::toJson() const {
    QJsonObject obj;
    obj["value"] = QString::fromStdString(value);
    return obj;
  }

  QJsonObject Expr::Int::toJson() const {
    QJsonObject obj;
    obj["value"] = value;
    return obj;
  }

  QJsonObject Expr::Float::toJson() const {
    QJsonObject obj;
    obj["value"] = value;
    return obj;
  }

  QJsonObject Expr::Call::toJson() const {
    QJsonObject obj;
    if (value)
      obj["value"] = value->toJson();
    return obj;
  }

  QJsonObject Expr::Neg::toJson() const {
    QJsonObject obj;
    if (value)
      obj["value"] = value->toJson();
    return obj;
  }

  QJsonObject Expr::Not::toJson() const {
    QJsonObject obj;
    if (value)
      obj["value"] = value->toJson();
    return obj;
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

  QJsonObject Expr::Paren::toJson() const {
    QJsonObject obj;
    if (value)
      obj["value"] = value->toJson();
    return obj;
  }
}
