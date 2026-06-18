#include "koda_ast_json.h"

namespace koda {

QJsonObject toJson(const System& system) {
    QJsonObject obj;
    QJsonArray compArray;
    for (const auto& component : system.components) {
        if (component)
            compArray.append(toJson(*component));
    }
    obj["components"] = compArray;
    return obj;
}

QJsonObject toJson(const Span& span) {
    QJsonObject obj;
    obj["lineStart"] = span.lineStart;
    obj["colStart"]  = span.colStart;
    obj["lineEnd"]   = span.lineEnd;
    obj["colEnd"]    = span.colEnd;
    return obj;
}

QJsonObject toJson(const Component& component) {
    QJsonObject obj;

    obj["kind"]  = QString::fromStdString(component.kindToString());
    obj["name"]  = QString::fromStdString(component.name);
    obj["srcId"] = QString::fromStdString(component.srcId);
    obj["span"]  = toJson(component.span);

    QJsonArray argsArray;
    for (const auto& arg : component.args) {
        if (arg)
            argsArray.append(toJson(*arg));
    }
    obj["args"] = argsArray;

    QJsonArray stmtArray;
    for (const auto& stmt : component.statements) {
        if (stmt)
            stmtArray.append(toJson(*stmt));
    }
    obj["statements"] = stmtArray;

    return obj;
}

QJsonObject toJson(const Argument& argument) {
    QJsonObject obj;
    obj["kind"]  = QString::fromStdString(argument.aKindToString());
    obj["a"]     = QString::fromStdString(argument.a);
    obj["b"]     = QString::fromStdString(argument.b);
    obj["srcId"] = QString::fromStdString(argument.srcId);
    obj["span"]  = toJson(argument.span);
    return obj;
}

QJsonObject toJson(const Statement& statement) {
    QJsonObject obj;
    std::visit([&obj] (const auto& ptr) {
      if (ptr)
        obj["node"] = toJson(*ptr);
    }, statement.node);
    obj["span"] = toJson(statement.span);
    return obj;
}

QJsonObject toJson(const Flow& flow) {
    QJsonObject obj;
    obj["name"]  = QString::fromStdString(flow.name);
    obj["srcId"] = QString::fromStdString(flow.srcId);

    QJsonArray tagsArray;
    for (const auto& tag : flow.tags) {
        tagsArray.append(QString::fromStdString(tag));
    }
    obj["tags"] = tagsArray;

    if (flow.strategy)
        obj["strategy"] = toJson(*flow.strategy);

    obj["span"] = toJson(flow.span);
    return obj;
}

QJsonObject toJson(const VarDef& varDef) {
    QJsonObject obj;
    obj["varType"] = QString::fromStdString(varDef.varType);
    obj["name"]    = QString::fromStdString(varDef.name);
    obj["srcId"]   = QString::fromStdString(varDef.srcId);
    if (varDef.init)
        obj["init"] = toJson(*varDef.init);
    if (varDef.fallback)
        obj["fallback"] = toJson(*varDef.fallback);
    obj["span"] = toJson(varDef.span);
    return obj;
}

QJsonObject toJson(const EventDef& eventDef) {
    QJsonObject obj;
    obj["typeName"] = QString::fromStdString(eventDef.typeName);
    obj["name"]     = QString::fromStdString(eventDef.name);
    obj["srcId"]    = QString::fromStdString(eventDef.srcId);

    QJsonArray argsArray;
    for (const auto& arg : eventDef.args) {
        if (arg)
            argsArray.append(toJson(*arg));
    }
    obj["args"] = argsArray;

    QJsonArray componentsArray;
    for (const auto& component : eventDef.components) {
        if (component)
            componentsArray.append(toJson(*component));
    }
    obj["components"] = componentsArray;

    obj["span"] = toJson(eventDef.span);
    return obj;
}

QJsonObject toJson(const EventDefComponent& edc) {
  QJsonObject obj;
  obj["kind"] = QString::fromStdString(edc.kind);
  obj["text"] = QString::fromStdString(edc.text);
  obj["span"] = toJson(edc.span);    // ADD
  return obj;
}

QJsonObject toJson(const EventCall& eventCall) {
    QJsonObject obj;
    obj["name"]     = QString::fromStdString(eventCall.name);
    obj["receiver"] = QString::fromStdString(eventCall.receiver);
    obj["srcId"]    = QString::fromStdString(eventCall.srcId);

    QJsonArray argsArray;
    for (const auto& arg : eventCall.args) {
        if (arg)
            argsArray.append(toJson(*arg));
    }
    obj["args"] = argsArray;
    obj["span"] = toJson(eventCall.span);
    return obj;
}

QJsonObject toJson(const RosDef& rosDef) {
    QJsonObject obj;
    obj["kind"] = QString::fromStdString(rosDef.rKindToString());
    if (rosDef.def)
        obj["def"] = toJson(*rosDef.def);
    obj["srcId"] = QString::fromStdString(rosDef.srcId);
    obj["span"]  = toJson(rosDef.span);
    return obj;
}

QJsonObject toJson(const ActionDef& actionDef) {
    QJsonObject obj;
    obj["kind"]   = QString::fromStdString(actionDef.adKindToString());
    obj["label1"] = QString::fromStdString(actionDef.label1);
    obj["label2"] = QString::fromStdString(actionDef.label2);
    obj["srcId"]  = QString::fromStdString(actionDef.srcId);

    QJsonArray rosDefsArray;
    for (const auto& rosDef : actionDef.rosDefs) {
        if (rosDef)
            rosDefsArray.append(toJson(*rosDef));
    }
    obj["rosDefs"] = rosDefsArray;
    obj["span"]    = toJson(actionDef.span);
    return obj;
}

QJsonObject toJson(const StrategyBlock& block) {
    QJsonObject obj;
    QJsonArray flowArray;
    for (const auto& flow : block.flows) {
        if (flow)
            flowArray.append(toJson(*flow));
    }
    obj["flows"] = flowArray;
    obj["span"]  = toJson(block.span);
    return obj;
}

QJsonObject toJson(const VarsBlock& block) {
    QJsonObject obj;
    QJsonArray varsArray;
    for (const auto& var : block.vars) {
        if (var)
            varsArray.append(toJson(*var));
    }
    obj["vars"] = varsArray;
    obj["span"] = toJson(block.span);
    return obj;
}

QJsonObject toJson(const Strategy& strategy) {
    QJsonObject obj;
    std::visit([&obj] (const auto& ptr) {
      if (ptr)
        obj["v"] = toJson(*ptr);
    }, strategy.v);
    obj["srcId"] = QString::fromStdString(strategy.srcId);
    obj["span"]  = toJson(strategy.span);
    return obj;
}

QJsonObject toJson(const StrategyHandler& handler) {
    QJsonObject obj;
    obj["kind"] = QString::fromStdString(handler.toString());
    if (handler.emitter)
        obj["emitter"] = toJson(*handler.emitter);
    if (handler.body)
        obj["body"] = toJson(*handler.body);
    obj["srcId"] = QString::fromStdString(handler.srcId);
    obj["span"]  = toJson(handler.span);
    return obj;
}

QJsonObject toJson(const Expr& expr) {
    QJsonObject obj;
    std::visit([&obj] (const auto& ptr) {
      if (ptr)
        obj["v"] = toJson(*ptr);
    }, expr.v);
    obj["srcId"] = QString::fromStdString(expr.srcId);
    obj["span"]  = toJson(expr.span);
    return obj;
}

// ---------------------------------------------------------------------------
// Strategy nested structs
// ---------------------------------------------------------------------------
QJsonObject toJson(const Strategy::Seq& seq) {
    QJsonObject obj;
    QJsonArray altsArray;
    for (const auto& alt : seq.alts) {
        if (alt) altsArray.append(toJson(*alt));
    }
    obj["alts"] = altsArray;
    obj["span"] = toJson(seq.span);   // ADD THIS
    return obj;
}

QJsonObject toJson(const Strategy::Join& join) {
    QJsonObject obj;
    QJsonArray altsArray;
    for (const auto& alt : join.alts) {
        if (alt) altsArray.append(toJson(*alt));
    }
    obj["alts"] = altsArray;
    obj["span"] = toJson(join.span);   // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Either& either) {
    QJsonObject obj;
    QJsonArray altsArray;
    for (const auto& alt : either.alts) {
        if (alt) altsArray.append(toJson(*alt));
    }
    obj["alts"] = altsArray;
    obj["span"] = toJson(either.span); // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Let& let) {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(let.name);
    if (let.call) obj["call"] = toJson(*let.call);
    obj["span"] = toJson(let.span);    // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Within& within) {
    QJsonObject obj;
    obj["seconds"] = within.seconds;
    if (within.a) obj["a"] = toJson(*within.a);
    if (within.b) obj["b"] = toJson(*within.b);
    QJsonArray handlersArray;
    for (const auto& handler : within.handlers) {
        if (handler) handlersArray.append(toJson(*handler));
    }
    obj["handlers"] = handlersArray;
    obj["span"] = toJson(within.span); // ADD
    return obj;
}

QJsonObject toJson(const Strategy::IfElse& ifElse) {
    QJsonObject obj;
    if (ifElse.cond) obj["cond"] = toJson(*ifElse.cond);
    if (ifElse.a)    obj["a"] = toJson(*ifElse.a);
    if (ifElse.b)    obj["b"] = toJson(*ifElse.b);
    obj["span"] = toJson(ifElse.span); // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Repeat& repeat) {
    QJsonObject obj;
    obj["seconds"] = repeat.seconds;
    obj["iterations"] = repeat.iterations;
    if (repeat.a) obj["a"] = toJson(*repeat.a);
    QJsonArray handlersArray;
    for (const auto& handler : repeat.handlers) {
        if (handler) handlersArray.append(toJson(*handler));
    }
    obj["handlers"] = handlersArray;
    obj["span"] = toJson(repeat.span); // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Guard& guard) {
    QJsonObject obj;
    if (guard.cond) obj["cond"] = toJson(*guard.cond);
    obj["span"] = toJson(guard.span);  // ADD
    return obj;
}

QJsonObject toJson(const Strategy::End& end) {
    QJsonObject obj;
    obj["span"] = toJson(end.span);    // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Continue& cont) {
    QJsonObject obj;
    obj["span"] = toJson(cont.span);   // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Ref& ref) {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(ref.name);
    obj["span"] = toJson(ref.span);    // ADD
    return obj;
}

QJsonObject toJson(const Strategy::TaskCall& taskCall) {
    QJsonObject obj;
    if (taskCall.call) obj["call"] = toJson(*taskCall.call);
    QJsonArray handlersArray;
    for (const auto& handler : taskCall.handlers) {
        if (handler) handlersArray.append(toJson(*handler));
    }
    obj["handlers"] = handlersArray;
    obj["span"] = toJson(taskCall.span); // ADD
    return obj;
}

QJsonObject toJson(const Strategy::Paren& paren) {
    QJsonObject obj;
    if (paren.a) obj["a"] = toJson(*paren.a);
    obj["span"] = toJson(paren.span);  // ADD
    return obj;
}

// ---------------------------------------------------------------------------
// Expr nested structs
// ---------------------------------------------------------------------------
QJsonObject toJson(const Expr::Id& id) {
  QJsonObject obj;
  obj["value"] = QString::fromStdString(id.value);
  obj["span"] = toJson(id.span);     // ADD
  return obj;
}

QJsonObject toJson(const Expr::Str& str) {
  QJsonObject obj;
  obj["value"] = QString::fromStdString(str.value);
  obj["span"] = toJson(str.span);    // ADD
  return obj;
}

QJsonObject toJson(const Expr::Int& i) {
  QJsonObject obj;
  obj["value"] = i.value;
  obj["span"] = toJson(i.span);      // ADD
  return obj;
}

QJsonObject toJson(const Expr::Float& f) {
  QJsonObject obj;
  obj["value"] = f.value;
  obj["span"] = toJson(f.span);      // ADD
  return obj;
}

QJsonObject toJson(const Expr::Call& call) {
  QJsonObject obj;
  if (call.value) obj["value"] = toJson(*call.value);
  obj["span"] = toJson(call.span);   // ADD
  return obj;
}

QJsonObject toJson(const Expr::Neg& neg) {
  QJsonObject obj;
  if (neg.value) obj["value"] = toJson(*neg.value);
  obj["span"] = toJson(neg.span);    // ADD
  return obj;
}

QJsonObject toJson(const Expr::Not& not_) {
  QJsonObject obj;
  if (not_.value) obj["value"] = toJson(*not_.value);
  obj["span"] = toJson(not_.span);   // ADD
  return obj;
}

QJsonObject toJson(const Expr::BinOp& binOp) {
  QJsonObject obj;
  obj["operation"] = QString::fromStdString(binOp.toString());
  if (binOp.a) obj["a"] = toJson(*binOp.a);
  if (binOp.b) obj["b"] = toJson(*binOp.b);
  obj["span"] = toJson(binOp.span);  // ADD
  return obj;
}

QJsonObject toJson(const Expr::Paren& paren) {
  QJsonObject obj;
  if (paren.value) obj["value"] = toJson(*paren.value);
  obj["span"] = toJson(paren.span);  // ADD
  return obj;
}

} // namespace koda