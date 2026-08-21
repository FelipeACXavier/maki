#pragma once

#include <any>

#include "ast/ast.h"
#include "error_listener.h"
#include "parser/KodaBaseVisitor.h"
#include "parser/KodaParser.h"
#include "typing/type_registry.h"

namespace koda
{
class CST2AST final : public KodaBaseVisitor
{
public:
  CST2AST(std::shared_ptr<types::TypeRegistry> registry, CollectingErrorListener* errorListener);

  // Entry: build a whole System
  System build(KodaParser::SystemContext* ctx);

  // -------------------------
  // Top-level
  // -------------------------

  std::any visitSystem(KodaParser::SystemContext* ctx) override;
  std::any visitTopLevelComponent(KodaParser::TopLevelComponentContext* ctx) override;
  std::any visitArgumentList(KodaParser::ArgumentListContext* ctx) override;
  std::any visitMappingDeclaration(KodaParser::MappingDeclarationContext* ctx) override;
  std::any visitArgPlain(KodaParser::ArgPlainContext* ctx) override;
  std::any visitArgReq(KodaParser::ArgReqContext* ctx) override;
  std::any visitArgPro(KodaParser::ArgProContext* ctx) override;

  // -------------------------
  // Statements
  // -------------------------

  std::any visitStatement(KodaParser::StatementContext* ctx) override;
  std::any visitTasksBlock(KodaParser::TasksBlockContext* ctx) override;
  std::any visitFlow(KodaParser::FlowContext* ctx) override;
  std::any visitIdentList(KodaParser::IdentListContext* ctx) override;
  std::any visitVarsBlock(KodaParser::VarsBlockContext* ctx) override;
  std::any visitVariableStatement(KodaParser::VariableStatementContext* ctx) override;

  // Action/service/topic blocks mapped to same IR (ActionDef)
  std::any visitActionBlock(KodaParser::ActionBlockContext* ctx) override;
  std::any visitServiceBlock(KodaParser::ServiceBlockContext* ctx) override;
  std::any visitTopicBlock(KodaParser::TopicBlockContext* ctx) override;

  // -------------------------
  // ROS defs
  // -------------------------
  std::any visitRosDefStatement(KodaParser::RosDefStatementContext* ctx) override;
  std::any visitEventDefStatement(KodaParser::EventDefStatementContext* ctx) override;

  // -------------------------
  // Strategy
  // -------------------------

  std::any visitStratSeq(KodaParser::StratSeqContext* ctx) override;
  std::any visitStratJoin(KodaParser::StratJoinContext* ctx) override;
  std::any visitStratEither(KodaParser::StratEitherContext* ctx) override;
  std::any visitStratWithin(KodaParser::StratWithinContext* ctx) override;
  std::any visitStratRepeat(KodaParser::StratRepeatContext* ctx) override;
  std::any visitStratEnd(KodaParser::StratEndContext* ctx) override;
  std::any visitStratContinue(KodaParser::StratContinueContext* ctx) override;
  std::any visitStratParen(KodaParser::StratParenContext* ctx) override;
  std::any visitStratTask(KodaParser::StratTaskContext* ctx) override;
  std::any visitHandlerOnError(KodaParser::HandlerOnErrorContext* ctx) override;
  std::any visitHandlerOnAbort(KodaParser::HandlerOnAbortContext* ctx) override;
  std::any visitHandlerOnEmitter(KodaParser::HandlerOnEmitterContext* ctx) override;

  // -------------------------
  // Event calls
  // -------------------------

  std::any visitEvCall(KodaParser::EvCallContext* ctx) override;
  std::any visitEvQualifiedCall(KodaParser::EvQualifiedCallContext* ctx) override;
  std::any visitExprList(KodaParser::ExprListContext* ctx) override;

  // -------------------------
  // Expressions
  // -------------------------
  std::any visitExprId(KodaParser::ExprIdContext* ctx) override;
  std::any visitIdentifier(KodaParser::IdentifierContext* ctx) override;
  std::any visitExprString(KodaParser::ExprStringContext* ctx) override;
  std::any visitExprInt(KodaParser::ExprIntContext* ctx) override;
  std::any visitExprFloat(KodaParser::ExprFloatContext* ctx) override;
  std::any visitExprBoolean(KodaParser::ExprBooleanContext* ctx) override;

  std::any visitExprCall(KodaParser::ExprCallContext* ctx) override;

  std::any visitExprParen(KodaParser::ExprParenContext* ctx) override;
  std::any visitExprRecord(KodaParser::ExprRecordContext* ctx) override;
  std::any visitRecordFieldInitializer(KodaParser::RecordFieldInitializerContext* ctx) override;
  std::any visitExprListLiteral(KodaParser::ExprListLiteralContext* ctx) override;
  std::any visitExprMapLiteral(KodaParser::ExprMapLiteralContext* ctx) override;
  std::any visitMapFieldInitializer(KodaParser::MapFieldInitializerContext* ctx) override;

  std::any visitCompOp(KodaParser::CompOpContext* ctx) override;
  std::any visitExprOr(KodaParser::ExprOrContext* ctx) override;
  std::any visitExprAnd(KodaParser::ExprAndContext* ctx) override;
  std::any visitExprCmp(KodaParser::ExprCmpContext* ctx) override;
  std::any visitExprNot(KodaParser::ExprNotContext* ctx) override;
  std::any visitExprAdd(KodaParser::ExprAddContext* ctx) override;
  std::any visitExprMul(KodaParser::ExprMulContext* ctx) override;
  std::any visitExprUnary(KodaParser::ExprUnaryContext* ctx) override;

private:
  std::shared_ptr<types::TypeRegistry> mTypeRegistry;
  CollectingErrorListener* mErrorListener;

  // Build ActionDef / ServiceDef / TopicDef using the same field extraction.
  template <typename CtxT>
  std::any buildActionLike(CtxT* ctx, ActionDef::Kind kind);

  bool containsContinue(PStrategy s);

  types::TypeDefinition convertTypeDeclaration(KodaParser::TypeDeclarationContext* ctx);
  types::TypeDefinition convertEnumDeclaration(KodaParser::EnumDeclarationContext* ctx);
  types::TypeReference convertTypeReference(KodaParser::TypeReferenceContext* ctx);
  types::QualifiedName convertQualifiedName(KodaParser::QualifiedNameContext* ctx);
  types::EnumUnderlyingKind convertEnumUnderlyingType(KodaParser::TypeReferenceContext* ctx);
};
}  // namespace koda