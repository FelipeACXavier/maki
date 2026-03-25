
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "KodaParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by KodaParser.
 */
class  KodaVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by KodaParser.
   */
    virtual std::any visitSystem(KodaParser::SystemContext *context) = 0;

    virtual std::any visitTopLevelComponent(KodaParser::TopLevelComponentContext *context) = 0;

    virtual std::any visitArgumentList(KodaParser::ArgumentListContext *context) = 0;

    virtual std::any visitArgPlain(KodaParser::ArgPlainContext *context) = 0;

    virtual std::any visitArgReq(KodaParser::ArgReqContext *context) = 0;

    virtual std::any visitArgPro(KodaParser::ArgProContext *context) = 0;

    virtual std::any visitStatement(KodaParser::StatementContext *context) = 0;

    virtual std::any visitTasksBlock(KodaParser::TasksBlockContext *context) = 0;

    virtual std::any visitFlow(KodaParser::FlowContext *context) = 0;

    virtual std::any visitIdentList(KodaParser::IdentListContext *context) = 0;

    virtual std::any visitVarsBlock(KodaParser::VarsBlockContext *context) = 0;

    virtual std::any visitVariableStatement(KodaParser::VariableStatementContext *context) = 0;

    virtual std::any visitActionBlock(KodaParser::ActionBlockContext *context) = 0;

    virtual std::any visitServiceBlock(KodaParser::ServiceBlockContext *context) = 0;

    virtual std::any visitTopicBlock(KodaParser::TopicBlockContext *context) = 0;

    virtual std::any visitRosDefStatement(KodaParser::RosDefStatementContext *context) = 0;

    virtual std::any visitEventDefStatement(KodaParser::EventDefStatementContext *context) = 0;

    virtual std::any visitEventDefComponentList(KodaParser::EventDefComponentListContext *context) = 0;

    virtual std::any visitEdcRosEvent(KodaParser::EdcRosEventContext *context) = 0;

    virtual std::any visitEdcTimeout(KodaParser::EdcTimeoutContext *context) = 0;

    virtual std::any visitEdcWhenAllowedIn(KodaParser::EdcWhenAllowedInContext *context) = 0;

    virtual std::any visitEdcReply(KodaParser::EdcReplyContext *context) = 0;

    virtual std::any visitEdcDependsAfter(KodaParser::EdcDependsAfterContext *context) = 0;

    virtual std::any visitEdcOnceIn(KodaParser::EdcOnceInContext *context) = 0;

    virtual std::any visitEdcStart(KodaParser::EdcStartContext *context) = 0;

    virtual std::any visitEdcReset(KodaParser::EdcResetContext *context) = 0;

    virtual std::any visitRosData(KodaParser::RosDataContext *context) = 0;

    virtual std::any visitWhenMode(KodaParser::WhenModeContext *context) = 0;

    virtual std::any visitTimeUnit(KodaParser::TimeUnitContext *context) = 0;

    virtual std::any visitStratRef(KodaParser::StratRefContext *context) = 0;

    virtual std::any visitStratIfElse(KodaParser::StratIfElseContext *context) = 0;

    virtual std::any visitStratRepeat(KodaParser::StratRepeatContext *context) = 0;

    virtual std::any visitStratEvery(KodaParser::StratEveryContext *context) = 0;

    virtual std::any visitStratParen(KodaParser::StratParenContext *context) = 0;

    virtual std::any visitStratGuard(KodaParser::StratGuardContext *context) = 0;

    virtual std::any visitStratSeq(KodaParser::StratSeqContext *context) = 0;

    virtual std::any visitStratTask(KodaParser::StratTaskContext *context) = 0;

    virtual std::any visitStratJoin(KodaParser::StratJoinContext *context) = 0;

    virtual std::any visitStratEither(KodaParser::StratEitherContext *context) = 0;

    virtual std::any visitStratLet(KodaParser::StratLetContext *context) = 0;

    virtual std::any visitStratEnd(KodaParser::StratEndContext *context) = 0;

    virtual std::any visitStratWithin(KodaParser::StratWithinContext *context) = 0;

    virtual std::any visitStratContinue(KodaParser::StratContinueContext *context) = 0;

    virtual std::any visitHandlerOnError(KodaParser::HandlerOnErrorContext *context) = 0;

    virtual std::any visitHandlerOnAbort(KodaParser::HandlerOnAbortContext *context) = 0;

    virtual std::any visitHandlerOnEmitter(KodaParser::HandlerOnEmitterContext *context) = 0;

    virtual std::any visitEvQualifiedCall(KodaParser::EvQualifiedCallContext *context) = 0;

    virtual std::any visitEvCall(KodaParser::EvCallContext *context) = 0;

    virtual std::any visitExprList(KodaParser::ExprListContext *context) = 0;

    virtual std::any visitExpression(KodaParser::ExpressionContext *context) = 0;

    virtual std::any visitExprOr(KodaParser::ExprOrContext *context) = 0;

    virtual std::any visitExprAnd(KodaParser::ExprAndContext *context) = 0;

    virtual std::any visitExprCmp(KodaParser::ExprCmpContext *context) = 0;

    virtual std::any visitCompOp(KodaParser::CompOpContext *context) = 0;

    virtual std::any visitExprNot(KodaParser::ExprNotContext *context) = 0;

    virtual std::any visitExprAdd(KodaParser::ExprAddContext *context) = 0;

    virtual std::any visitExprMul(KodaParser::ExprMulContext *context) = 0;

    virtual std::any visitExprUnary(KodaParser::ExprUnaryContext *context) = 0;

    virtual std::any visitExprCall(KodaParser::ExprCallContext *context) = 0;

    virtual std::any visitExprId(KodaParser::ExprIdContext *context) = 0;

    virtual std::any visitExprString(KodaParser::ExprStringContext *context) = 0;

    virtual std::any visitExprInt(KodaParser::ExprIntContext *context) = 0;

    virtual std::any visitExprFloat(KodaParser::ExprFloatContext *context) = 0;

    virtual std::any visitExprParen(KodaParser::ExprParenContext *context) = 0;

    virtual std::any visitIdentifier(KodaParser::IdentifierContext *context) = 0;


};

