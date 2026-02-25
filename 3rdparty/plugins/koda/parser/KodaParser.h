
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaParser : public antlr4::Parser {
public:
  enum {
    TASK = 1, CAPABILITY = 2, STRATEGY = 3, VARS = 4, ACTION = 5, SERVICE = 6, 
    TOPIC = 7, TRIGGER = 8, RETURN = 9, ABORT = 10, ERROR = 11, IN = 12, 
    OUT = 13, ON = 14, REQ = 15, PRO = 16, END = 17, REPEAT = 18, JOIN = 19, 
    EITHER = 20, LET = 21, WITHIN = 22, DO = 23, ELSE = 24, IF = 25, THEN = 26, 
    GUARD = 27, EVERY = 28, TIMEOUT = 29, ALLOWED = 30, REPLY = 31, AFTER = 32, 
    ONCE = 33, ALWAYS = 34, MISSION = 35, IDLE = 36, S = 37, MS = 38, US = 39, 
    NS = 40, ARROW = 41, PIPE = 42, EQ = 43, NEQ = 44, LEQ = 45, GEQ = 46, 
    LT = 47, GT = 48, ASSIGN = 49, COLON = 50, COMMA = 51, SEMI = 52, DOT = 53, 
    PLUS = 54, MINUS = 55, STAR = 56, SLASH = 57, NOT = 58, AND = 59, OR = 60, 
    LPAREN = 61, RPAREN = 62, LBRACE = 63, RBRACE = 64, LBRACK = 65, RBRACK = 66, 
    NATURAL = 67, REAL = 68, IDENT = 69, STRING = 70, ANY = 71, LINE_COMMENT = 72, 
    BLOCK_COMMENT = 73, WS = 74
  };

  enum {
    RuleSystem = 0, RuleTopLevelComponent = 1, RuleArgumentList = 2, RuleArgument = 3, 
    RuleStatement = 4, RuleTasksBlock = 5, RuleFlow = 6, RuleIdentList = 7, 
    RuleVarsBlock = 8, RuleVariableStatement = 9, RuleActionBlock = 10, 
    RuleServiceBlock = 11, RuleTopicBlock = 12, RuleRosDefStatement = 13, 
    RuleEventDefStatement = 14, RuleEventDefComponentList = 15, RuleEventDefComponent = 16, 
    RuleRosData = 17, RuleWhenMode = 18, RuleTimeUnit = 19, RuleStrategy = 20, 
    RuleStrategyHandler = 21, RuleEventStatement = 22, RuleExprList = 23, 
    RuleExpression = 24, RuleExprOr = 25, RuleExprAnd = 26, RuleExprCmp = 27, 
    RuleCompOp = 28, RuleExprNot = 29, RuleExprAdd = 30, RuleExprMul = 31, 
    RuleExprUnary = 32, RuleExprPrimary = 33, RuleIdentifier = 34
  };

  explicit KodaParser(antlr4::TokenStream *input);

  KodaParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~KodaParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class SystemContext;
  class TopLevelComponentContext;
  class ArgumentListContext;
  class ArgumentContext;
  class StatementContext;
  class TasksBlockContext;
  class FlowContext;
  class IdentListContext;
  class VarsBlockContext;
  class VariableStatementContext;
  class ActionBlockContext;
  class ServiceBlockContext;
  class TopicBlockContext;
  class RosDefStatementContext;
  class EventDefStatementContext;
  class EventDefComponentListContext;
  class EventDefComponentContext;
  class RosDataContext;
  class WhenModeContext;
  class TimeUnitContext;
  class StrategyContext;
  class StrategyHandlerContext;
  class EventStatementContext;
  class ExprListContext;
  class ExpressionContext;
  class ExprOrContext;
  class ExprAndContext;
  class ExprCmpContext;
  class CompOpContext;
  class ExprNotContext;
  class ExprAddContext;
  class ExprMulContext;
  class ExprUnaryContext;
  class ExprPrimaryContext;
  class IdentifierContext; 

  class  SystemContext : public antlr4::ParserRuleContext {
  public:
    SystemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<TopLevelComponentContext *> topLevelComponent();
    TopLevelComponentContext* topLevelComponent(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SystemContext* system();

  class  TopLevelComponentContext : public antlr4::ParserRuleContext {
  public:
    TopLevelComponentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TASK();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    ArgumentListContext *argumentList();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);
    antlr4::tree::TerminalNode *CAPABILITY();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopLevelComponentContext* topLevelComponent();

  class  ArgumentListContext : public antlr4::ParserRuleContext {
  public:
    ArgumentListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ArgumentContext *> argument();
    ArgumentContext* argument(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgumentListContext* argumentList();

  class  ArgumentContext : public antlr4::ParserRuleContext {
  public:
    ArgumentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ArgumentContext() = default;
    void copyFrom(ArgumentContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ArgPlainContext : public ArgumentContext {
  public:
    ArgPlainContext(ArgumentContext *ctx);

    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArgReqContext : public ArgumentContext {
  public:
    ArgReqContext(ArgumentContext *ctx);

    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *REQ();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArgProContext : public ArgumentContext {
  public:
    ArgProContext(ArgumentContext *ctx);

    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *PRO();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ArgumentContext* argument();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TasksBlockContext *tasksBlock();
    VarsBlockContext *varsBlock();
    ActionBlockContext *actionBlock();
    ServiceBlockContext *serviceBlock();
    TopicBlockContext *topicBlock();
    RosDefStatementContext *rosDefStatement();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementContext* statement();

  class  TasksBlockContext : public antlr4::ParserRuleContext {
  public:
    TasksBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRATEGY();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<FlowContext *> flow();
    FlowContext* flow(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TasksBlockContext* tasksBlock();

  class  FlowContext : public antlr4::ParserRuleContext {
  public:
    FlowContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *COLON();
    StrategyContext *strategy();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LBRACK();
    antlr4::tree::TerminalNode *RBRACK();
    IdentListContext *identList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FlowContext* flow();

  class  IdentListContext : public antlr4::ParserRuleContext {
  public:
    IdentListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentListContext* identList();

  class  VarsBlockContext : public antlr4::ParserRuleContext {
  public:
    VarsBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VARS();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<VariableStatementContext *> variableStatement();
    VariableStatementContext* variableStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VarsBlockContext* varsBlock();

  class  VariableStatementContext : public antlr4::ParserRuleContext {
  public:
    VariableStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *ASSIGN();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COLON();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableStatementContext* variableStatement();

  class  ActionBlockContext : public antlr4::ParserRuleContext {
  public:
    ActionBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ACTION();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<RosDefStatementContext *> rosDefStatement();
    RosDefStatementContext* rosDefStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ActionBlockContext* actionBlock();

  class  ServiceBlockContext : public antlr4::ParserRuleContext {
  public:
    ServiceBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SERVICE();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<RosDefStatementContext *> rosDefStatement();
    RosDefStatementContext* rosDefStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ServiceBlockContext* serviceBlock();

  class  TopicBlockContext : public antlr4::ParserRuleContext {
  public:
    TopicBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TOPIC();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<RosDefStatementContext *> rosDefStatement();
    RosDefStatementContext* rosDefStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopicBlockContext* topicBlock();

  class  RosDefStatementContext : public antlr4::ParserRuleContext {
  public:
    RosDefStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TRIGGER();
    antlr4::tree::TerminalNode *COLON();
    EventDefStatementContext *eventDefStatement();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *RETURN();
    antlr4::tree::TerminalNode *ABORT();
    antlr4::tree::TerminalNode *ERROR();
    antlr4::tree::TerminalNode *IN();
    antlr4::tree::TerminalNode *OUT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RosDefStatementContext* rosDefStatement();

  class  EventDefStatementContext : public antlr4::ParserRuleContext {
  public:
    EventDefStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ArgumentListContext *argumentList();
    antlr4::tree::TerminalNode *COLON();
    EventDefComponentListContext *eventDefComponentList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EventDefStatementContext* eventDefStatement();

  class  EventDefComponentListContext : public antlr4::ParserRuleContext {
  public:
    EventDefComponentListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<EventDefComponentContext *> eventDefComponent();
    EventDefComponentContext* eventDefComponent(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EventDefComponentListContext* eventDefComponentList();

  class  EventDefComponentContext : public antlr4::ParserRuleContext {
  public:
    EventDefComponentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    EventDefComponentContext() = default;
    void copyFrom(EventDefComponentContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  EdcOnceInContext : public EventDefComponentContext {
  public:
    EdcOnceInContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *ONCE();
    antlr4::tree::TerminalNode *IN();
    WhenModeContext *whenMode();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcStartContext : public EventDefComponentContext {
  public:
    EdcStartContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *TRIGGER();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcResetContext : public EventDefComponentContext {
  public:
    EdcResetContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *ABORT();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcDependsAfterContext : public EventDefComponentContext {
  public:
    EdcDependsAfterContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *AFTER();
    antlr4::tree::TerminalNode *IDENT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcWhenAllowedInContext : public EventDefComponentContext {
  public:
    EdcWhenAllowedInContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *ALLOWED();
    antlr4::tree::TerminalNode *IN();
    WhenModeContext *whenMode();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcTimeoutContext : public EventDefComponentContext {
  public:
    EdcTimeoutContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *TIMEOUT();
    antlr4::tree::TerminalNode *NATURAL();
    TimeUnitContext *timeUnit();
    antlr4::tree::TerminalNode *ARROW();
    antlr4::tree::TerminalNode *IDENT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcRosEventContext : public EventDefComponentContext {
  public:
    EdcRosEventContext(EventDefComponentContext *ctx);

    RosDataContext *rosData();
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EdcReplyContext : public EventDefComponentContext {
  public:
    EdcReplyContext(EventDefComponentContext *ctx);

    antlr4::tree::TerminalNode *REPLY();
    antlr4::tree::TerminalNode *IDENT();
    WhenModeContext *whenMode();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  EventDefComponentContext* eventDefComponent();

  class  RosDataContext : public antlr4::ParserRuleContext {
  public:
    RosDataContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TOPIC();
    antlr4::tree::TerminalNode *SERVICE();
    antlr4::tree::TerminalNode *ACTION();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RosDataContext* rosData();

  class  WhenModeContext : public antlr4::ParserRuleContext {
  public:
    WhenModeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALWAYS();
    antlr4::tree::TerminalNode *MISSION();
    antlr4::tree::TerminalNode *IDLE();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  WhenModeContext* whenMode();

  class  TimeUnitContext : public antlr4::ParserRuleContext {
  public:
    TimeUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *S();
    antlr4::tree::TerminalNode *MS();
    antlr4::tree::TerminalNode *US();
    antlr4::tree::TerminalNode *NS();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TimeUnitContext* timeUnit();

  class  StrategyContext : public antlr4::ParserRuleContext {
  public:
    StrategyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    StrategyContext() = default;
    void copyFrom(StrategyContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  StratRefContext : public StrategyContext {
  public:
    StratRefContext(StrategyContext *ctx);

    IdentifierContext *identifier();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratIfElseContext : public StrategyContext {
  public:
    StratIfElseContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *IF();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *THEN();
    std::vector<StrategyContext *> strategy();
    StrategyContext* strategy(size_t i);
    antlr4::tree::TerminalNode *ELSE();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratRepeatContext : public StrategyContext {
  public:
    StratRepeatContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *REPEAT();
    antlr4::tree::TerminalNode *LPAREN();
    StrategyContext *strategy();
    antlr4::tree::TerminalNode *RPAREN();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratEveryContext : public StrategyContext {
  public:
    StratEveryContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *EVERY();
    antlr4::tree::TerminalNode *NATURAL();
    antlr4::tree::TerminalNode *LBRACE();
    StrategyContext *strategy();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<StrategyHandlerContext *> strategyHandler();
    StrategyHandlerContext* strategyHandler(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratParenContext : public StrategyContext {
  public:
    StratParenContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *LPAREN();
    StrategyContext *strategy();
    antlr4::tree::TerminalNode *RPAREN();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratGuardContext : public StrategyContext {
  public:
    StratGuardContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *GUARD();
    antlr4::tree::TerminalNode *LBRACE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RBRACE();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratSeqContext : public StrategyContext {
  public:
    StratSeqContext(StrategyContext *ctx);

    std::vector<StrategyContext *> strategy();
    StrategyContext* strategy(size_t i);
    antlr4::tree::TerminalNode *ARROW();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratTaskContext : public StrategyContext {
  public:
    StratTaskContext(StrategyContext *ctx);

    EventStatementContext *eventStatement();
    std::vector<StrategyHandlerContext *> strategyHandler();
    StrategyHandlerContext* strategyHandler(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratJoinContext : public StrategyContext {
  public:
    StratJoinContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *JOIN();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<StrategyContext *> strategy();
    StrategyContext* strategy(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> PIPE();
    antlr4::tree::TerminalNode* PIPE(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratEitherContext : public StrategyContext {
  public:
    StratEitherContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *EITHER();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<StrategyContext *> strategy();
    StrategyContext* strategy(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> PIPE();
    antlr4::tree::TerminalNode* PIPE(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratLetContext : public StrategyContext {
  public:
    StratLetContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *LET();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *ASSIGN();
    EventStatementContext *eventStatement();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratEndContext : public StrategyContext {
  public:
    StratEndContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *END();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  StratWithinContext : public StrategyContext {
  public:
    StratWithinContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *WITHIN();
    antlr4::tree::TerminalNode *NATURAL();
    antlr4::tree::TerminalNode *DO();
    std::vector<StrategyContext *> strategy();
    StrategyContext* strategy(size_t i);
    antlr4::tree::TerminalNode *ELSE();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  StrategyContext* strategy();
  StrategyContext* strategy(int precedence);
  class  StrategyHandlerContext : public antlr4::ParserRuleContext {
  public:
    StrategyHandlerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    StrategyHandlerContext() = default;
    void copyFrom(StrategyHandlerContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  HandlerOnErrorContext : public StrategyHandlerContext {
  public:
    HandlerOnErrorContext(StrategyHandlerContext *ctx);

    antlr4::tree::TerminalNode *ON();
    antlr4::tree::TerminalNode *ERROR();
    StrategyContext *strategy();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  HandlerOnAbortContext : public StrategyHandlerContext {
  public:
    HandlerOnAbortContext(StrategyHandlerContext *ctx);

    antlr4::tree::TerminalNode *ON();
    antlr4::tree::TerminalNode *ABORT();
    StrategyContext *strategy();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  HandlerOnEmitterContext : public StrategyHandlerContext {
  public:
    HandlerOnEmitterContext(StrategyHandlerContext *ctx);

    antlr4::tree::TerminalNode *ON();
    EventStatementContext *eventStatement();
    StrategyContext *strategy();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  StrategyHandlerContext* strategyHandler();

  class  EventStatementContext : public antlr4::ParserRuleContext {
  public:
    EventStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    EventStatementContext() = default;
    void copyFrom(EventStatementContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  EvCallContext : public EventStatementContext {
  public:
    EvCallContext(EventStatementContext *ctx);

    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ExprListContext *exprList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  EvQualifiedCallContext : public EventStatementContext {
  public:
    EvQualifiedCallContext(EventStatementContext *ctx);

    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ExprListContext *exprList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  EventStatementContext* eventStatement();

  class  ExprListContext : public antlr4::ParserRuleContext {
  public:
    ExprListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprListContext* exprList();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprOrContext *exprOr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();

  class  ExprOrContext : public antlr4::ParserRuleContext {
  public:
    ExprOrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprAndContext *> exprAnd();
    ExprAndContext* exprAnd(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OR();
    antlr4::tree::TerminalNode* OR(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprOrContext* exprOr();

  class  ExprAndContext : public antlr4::ParserRuleContext {
  public:
    ExprAndContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprCmpContext *> exprCmp();
    ExprCmpContext* exprCmp(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AND();
    antlr4::tree::TerminalNode* AND(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprAndContext* exprAnd();

  class  ExprCmpContext : public antlr4::ParserRuleContext {
  public:
    ExprCmpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprNotContext *> exprNot();
    ExprNotContext* exprNot(size_t i);
    CompOpContext *compOp();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprCmpContext* exprCmp();

  class  CompOpContext : public antlr4::ParserRuleContext {
  public:
    CompOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *NEQ();
    antlr4::tree::TerminalNode *LEQ();
    antlr4::tree::TerminalNode *GEQ();
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *GT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CompOpContext* compOp();

  class  ExprNotContext : public antlr4::ParserRuleContext {
  public:
    ExprNotContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NOT();
    ExprNotContext *exprNot();
    ExprAddContext *exprAdd();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprNotContext* exprNot();

  class  ExprAddContext : public antlr4::ParserRuleContext {
  public:
    ExprAddContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprMulContext *> exprMul();
    ExprMulContext* exprMul(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PLUS();
    antlr4::tree::TerminalNode* PLUS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MINUS();
    antlr4::tree::TerminalNode* MINUS(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprAddContext* exprAdd();

  class  ExprMulContext : public antlr4::ParserRuleContext {
  public:
    ExprMulContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprUnaryContext *> exprUnary();
    ExprUnaryContext* exprUnary(size_t i);
    std::vector<antlr4::tree::TerminalNode *> STAR();
    antlr4::tree::TerminalNode* STAR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SLASH();
    antlr4::tree::TerminalNode* SLASH(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprMulContext* exprMul();

  class  ExprUnaryContext : public antlr4::ParserRuleContext {
  public:
    ExprUnaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MINUS();
    ExprUnaryContext *exprUnary();
    ExprPrimaryContext *exprPrimary();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprUnaryContext* exprUnary();

  class  ExprPrimaryContext : public antlr4::ParserRuleContext {
  public:
    ExprPrimaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ExprPrimaryContext() = default;
    void copyFrom(ExprPrimaryContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ExprIntContext : public ExprPrimaryContext {
  public:
    ExprIntContext(ExprPrimaryContext *ctx);

    antlr4::tree::TerminalNode *NATURAL();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprParenContext : public ExprPrimaryContext {
  public:
    ExprParenContext(ExprPrimaryContext *ctx);

    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprCallContext : public ExprPrimaryContext {
  public:
    ExprCallContext(ExprPrimaryContext *ctx);

    EventStatementContext *eventStatement();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprStringContext : public ExprPrimaryContext {
  public:
    ExprStringContext(ExprPrimaryContext *ctx);

    antlr4::tree::TerminalNode *STRING();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprIdContext : public ExprPrimaryContext {
  public:
    ExprIdContext(ExprPrimaryContext *ctx);

    antlr4::tree::TerminalNode *IDENT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprFloatContext : public ExprPrimaryContext {
  public:
    ExprFloatContext(ExprPrimaryContext *ctx);

    antlr4::tree::TerminalNode *REAL();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ExprPrimaryContext* exprPrimary();

  class  IdentifierContext : public antlr4::ParserRuleContext {
  public:
    IdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *ABORT();
    antlr4::tree::TerminalNode *TRIGGER();
    antlr4::tree::TerminalNode *RETURN();
    antlr4::tree::TerminalNode *ERROR();
    antlr4::tree::TerminalNode *IN();
    antlr4::tree::TerminalNode *OUT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentifierContext* identifier();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool strategySempred(StrategyContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

