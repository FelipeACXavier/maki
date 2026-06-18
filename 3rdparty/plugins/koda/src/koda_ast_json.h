#pragma once

#include "ast/ast.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonArray>
#include <QString>

namespace koda
{
QJsonObject System::toJson();

QJsonObject Span::toJson();

QJsonObject Span::toJson();
QJsonObject Argument::toJson();
QJsonObject Statement::toJson();

QJsonObject Flow::toJson();
QJsonObject VarDef::toJson();

QJsonObject EventDef::toJson();
QJsonObject EventDefComponent::toJson();
QJsonObject EventCall::toJson();

QJsonObject RosDef::toJson();
QJsonObject ActionDef::toJson();

QJsonObject StrategyBlock::toJson();
QJsonObject VarsBlock::toJson();

QJsonObject Strategy::toJson();
QJsonObject StrategyHandler::toJson();

QJsonObject Expr::toJson();

// Strategy node helpers
QJsonObject Strategy::Seq::toJson();
QJsonObject Strategy::Join::toJson();
QJsonObject Strategy::Either::toJson();
QJsonObject Strategy::Let::toJson();
QJsonObject Strategy::Within::toJson();
QJsonObject Strategy::IfElse::toJson();
QJsonObject Strategy::Repeat::toJson();
QJsonObject Strategy::Guard::toJson();
QJsonObject Strategy::End::toJson();
QJsonObject Strategy::Continue::toJson();
QJsonObject Strategy::Ref::toJson();
QJsonObject Strategy::TaskCall::toJson();
QJsonObject Strategy::Paren::toJson();

// Expr nodes
QJsonObject Expr::Id::toJson();
QJsonObject Expr::Str::toJson();
QJsonObject Expr::Int::toJson();
QJsonObject Expr::Float::toJson();
QJsonObject Expr::Call::toJson();
QJsonObject Expr::Neg::toJson();
QJsonObject Expr::Not::toJson();
QJsonObject Expr::BinOp::toJson();
QJsonObject Expr::Paren::toJson();
}