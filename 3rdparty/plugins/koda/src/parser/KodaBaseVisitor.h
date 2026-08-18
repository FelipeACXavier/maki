
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "KodaVisitor.h"


/**
 * This class provides an empty implementation of KodaVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  KodaBaseVisitor : public KodaVisitor {
public:

  virtual std::any visitSystem(KodaParser::SystemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTopLevelDeclaration(KodaParser::TopLevelDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTopLevelComponent(KodaParser::TopLevelComponentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeRecord(KodaParser::TypeRecordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeAlias(KodaParser::TypeAliasContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFieldDeclaration(KodaParser::FieldDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumDeclaration(KodaParser::EnumDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumValue(KodaParser::EnumValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumLiteral(KodaParser::EnumLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMappingDeclaration(KodaParser::MappingDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeNamed(KodaParser::TypeNamedContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeList(KodaParser::TypeListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeOptional(KodaParser::TypeOptionalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeMap(KodaParser::TypeMapContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQualifiedName(KodaParser::QualifiedNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgumentList(KodaParser::ArgumentListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgPlain(KodaParser::ArgPlainContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgReq(KodaParser::ArgReqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgPro(KodaParser::ArgProContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(KodaParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTasksBlock(KodaParser::TasksBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFlow(KodaParser::FlowContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentList(KodaParser::IdentListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarsBlock(KodaParser::VarsBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableStatement(KodaParser::VariableStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitActionBlock(KodaParser::ActionBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitServiceBlock(KodaParser::ServiceBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTopicBlock(KodaParser::TopicBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRosDefStatement(KodaParser::RosDefStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReqDefStatement(KodaParser::ReqDefStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEventDefStatement(KodaParser::EventDefStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratRef(KodaParser::StratRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratSeq(KodaParser::StratSeqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratTask(KodaParser::StratTaskContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratJoin(KodaParser::StratJoinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratEither(KodaParser::StratEitherContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratEnd(KodaParser::StratEndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratWithin(KodaParser::StratWithinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratRepeat(KodaParser::StratRepeatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratContinue(KodaParser::StratContinueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStratParen(KodaParser::StratParenContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHandlerOnError(KodaParser::HandlerOnErrorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHandlerOnAbort(KodaParser::HandlerOnAbortContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHandlerOnEmitter(KodaParser::HandlerOnEmitterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvQualifiedCall(KodaParser::EvQualifiedCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvCall(KodaParser::EvCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprList(KodaParser::ExprListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(KodaParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprOr(KodaParser::ExprOrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprAnd(KodaParser::ExprAndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprCmp(KodaParser::ExprCmpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCompOp(KodaParser::CompOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprNot(KodaParser::ExprNotContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprAdd(KodaParser::ExprAddContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprMul(KodaParser::ExprMulContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprUnary(KodaParser::ExprUnaryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprCall(KodaParser::ExprCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprId(KodaParser::ExprIdContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprString(KodaParser::ExprStringContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprInt(KodaParser::ExprIntContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprFloat(KodaParser::ExprFloatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprParen(KodaParser::ExprParenContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(KodaParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }


};

