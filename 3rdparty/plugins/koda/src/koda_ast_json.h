#pragma once

#include "ast/ast.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QString>

namespace koda {

// Core types
QJsonObject toJson(const Span& span);
QJsonObject toJson(const Component& component);
QJsonObject toJson(const Argument& argument);
QJsonObject toJson(const Statement& statement);
QJsonObject toJson(const Flow& flow);
QJsonObject toJson(const VarDef& varDef);
QJsonObject toJson(const EventDef& eventDef);
QJsonObject toJson(const EventDefComponent& edc);
QJsonObject toJson(const EventCall& eventCall);
QJsonObject toJson(const RosDef& rosDef);
QJsonObject toJson(const ActionDef& actionDef);
QJsonObject toJson(const StrategyBlock& block);
QJsonObject toJson(const VarsBlock& block);
QJsonObject toJson(const Strategy& strategy);
QJsonObject toJson(const StrategyHandler& handler);
QJsonObject toJson(const Expr& expr);
QJsonObject toJson(const System& system);

// Strategy nested structs
QJsonObject toJson(const Strategy::Seq& seq);
QJsonObject toJson(const Strategy::Join& join);
QJsonObject toJson(const Strategy::Either& either);
QJsonObject toJson(const Strategy::Let& let);
QJsonObject toJson(const Strategy::Within& within);
QJsonObject toJson(const Strategy::IfElse& ifElse);
QJsonObject toJson(const Strategy::Repeat& repeat);
QJsonObject toJson(const Strategy::Guard& guard);
QJsonObject toJson(const Strategy::End& end);
QJsonObject toJson(const Strategy::Continue& cont);
QJsonObject toJson(const Strategy::Ref& ref);
QJsonObject toJson(const Strategy::TaskCall& taskCall);
QJsonObject toJson(const Strategy::Paren& paren);

// Expr nested structs
QJsonObject toJson(const Expr::Id& id);
QJsonObject toJson(const Expr::Str& str);
QJsonObject toJson(const Expr::Int& i);
QJsonObject toJson(const Expr::Float& f);
QJsonObject toJson(const Expr::Call& call);
QJsonObject toJson(const Expr::Neg& neg);
QJsonObject toJson(const Expr::Not& not_);
QJsonObject toJson(const Expr::BinOp& binOp);
QJsonObject toJson(const Expr::Paren& paren);

} // namespace koda