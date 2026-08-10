
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaParser : public antlr4::Parser {
public:
  enum {
    TASK = 1, CAPABILITY = 2, TYPE = 3, ENUM = 4, EXTENDS = 5, LIST = 6, 
    OPTIONAL = 7, MAP = 8, STRATEGY = 9, VARS = 10, ACTION = 11, SERVICE = 12, 
    TOPIC = 13, TRIGGER = 14, RETURN = 15, ABORT = 16, ERROR = 17, IN = 18, 
    OUT = 19, ON = 20, REQ = 21, PRO = 22, END = 23, CONTINUE = 24, REPEAT = 25, 
    JOIN = 26, EITHER = 27, LET = 28, WITHIN = 29, DO = 30, ELSE = 31, IF = 32, 
    THEN = 33, GUARD = 34, EVERY = 35, TIMEOUT = 36, ALLOWED = 37, REPLY = 38, 
    AFTER = 39, ONCE = 40, ALWAYS = 41, MISSION = 42, IDLE = 43, S = 44, 
    MS = 45, US = 46, NS = 47, ARROW = 48, PIPE = 49, EQ = 50, NEQ = 51, 
    LEQ = 52, GEQ = 53, LT = 54, GT = 55, ASSIGN = 56, DOUBLE_COLON = 57, 
    COLON = 58, COMMA = 59, SEMI = 60, DOT = 61, PLUS = 62, MINUS = 63, 
    STAR = 64, SLASH = 65, NOT = 66, AND = 67, OR = 68, LPAREN = 69, RPAREN = 70, 
    LBRACE = 71, RBRACE = 72, LBRACK = 73, RBRACK = 74, NATURAL = 75, REAL = 76, 
    IDENT = 77, STRING = 78, ANY = 79, LINE_COMMENT = 80, BLOCK_COMMENT = 81, 
    WS = 82
  };

  enum {
    RuleSystem = 0, RuleTopLevelDeclaration = 1, RuleTopLevelComponent = 2, 
    RuleTypeDeclaration = 3, RuleEnumDeclaration = 4, RuleFieldDeclaration = 5, 
    RuleEnumValue = 6, RuleEnumLiteral = 7, RuleTypeReference = 8, RuleQualifiedName = 9, 
    RuleArgumentList = 10, RuleArgument = 11, RuleStatement = 12, RuleTasksBlock = 13, 
    RuleFlow = 14, RuleIdentList = 15, RuleVarsBlock = 16, RuleVariableStatement = 17, 
    RuleActionBlock = 18, RuleServiceBlock = 19, RuleTopicBlock = 20, RuleRosDefStatement = 21, 
    RuleEventDefStatement = 22, RuleEventDefComponentList = 23, RuleEventDefComponent = 24, 
    RuleRosData = 25, RuleWhenMode = 26, RuleTimeUnit = 27, RuleStrategy = 28, 
    RuleStrategyHandler = 29, RuleEventStatement = 30, RuleExprList = 31, 
    RuleExpression = 32, RuleExprOr = 33, RuleExprAnd = 34, RuleExprCmp = 35, 
    RuleCompOp = 36, RuleExprNot = 37, RuleExprAdd = 38, RuleExprMul = 39, 
    RuleExprUnary = 40, RuleExprPrimary = 41, RuleIdentifier = 42
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
  class TopLevelDeclarationContext;
  class TopLevelComponentContext;
  class TypeDeclarationContext;
  class EnumDeclarationContext;
  class FieldDeclarationContext;
  class EnumValueContext;
  class EnumLiteralContext;
  class TypeReferenceContext;
  class QualifiedNameContext;
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
    std::vector<TopLevelDeclarationContext *> topLevelDeclaration();
    TopLevelDeclarationContext* topLevelDeclaration(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SystemContext* system();

  class  TopLevelDeclarationContext : public antlr4::ParserRuleContext {
  public:
    TopLevelDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TopLevelComponentContext *topLevelComponent();
    TypeDeclarationContext *typeDeclaration();
    EnumDeclarationContext *enumDeclaration();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopLevelDeclarationContext* topLevelDeclaration();

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

  class  TypeDeclarationContext : public antlr4::ParserRuleContext {
  public:
    TypeDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    TypeDeclarationContext() = default;
    void copyFrom(TypeDeclarationContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  TypeAliasContext : public TypeDeclarationContext {
  public:
    TypeAliasContext(TypeDeclarationContext *ctx);

    antlr4::tree::TerminalNode *TYPE();
    QualifiedNameContext *qualifiedName();
    antlr4::tree::TerminalNode *ASSIGN();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *SEMI();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TypeRecordContext : public TypeDeclarationContext {
  public:
    TypeRecordContext(TypeDeclarationContext *ctx);

    antlr4::tree::TerminalNode *TYPE();
    std::vector<QualifiedNameContext *> qualifiedName();
    QualifiedNameContext* qualifiedName(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *EXTENDS();
    std::vector<FieldDeclarationContext *> fieldDeclaration();
    FieldDeclarationContext* fieldDeclaration(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  TypeDeclarationContext* typeDeclaration();

  class  EnumDeclarationContext : public antlr4::ParserRuleContext {
  public:
    EnumDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ENUM();
    QualifiedNameContext *qualifiedName();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *COLON();
    TypeReferenceContext *typeReference();
    std::vector<EnumValueContext *> enumValue();
    EnumValueContext* enumValue(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumDeclarationContext* enumDeclaration();

  class  FieldDeclarationContext : public antlr4::ParserRuleContext {
  public:
    FieldDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *COLON();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *SEMI();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FieldDeclarationContext* fieldDeclaration();

  class  EnumValueContext : public antlr4::ParserRuleContext {
  public:
    EnumValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *ASSIGN();
    EnumLiteralContext *enumLiteral();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumValueContext* enumValue();

  class  EnumLiteralContext : public antlr4::ParserRuleContext {
  public:
    EnumLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *NATURAL();
    antlr4::tree::TerminalNode *MINUS();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumLiteralContext* enumLiteral();

  class  TypeReferenceContext : public antlr4::ParserRuleContext {
  public:
    TypeReferenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    TypeReferenceContext() = default;
    void copyFrom(TypeReferenceContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  TypeNamedContext : public TypeReferenceContext {
  public:
    TypeNamedContext(TypeReferenceContext *ctx);

    QualifiedNameContext *qualifiedName();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TypeListContext : public TypeReferenceContext {
  public:
    TypeListContext(TypeReferenceContext *ctx);

    antlr4::tree::TerminalNode *LIST();
    antlr4::tree::TerminalNode *LT();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *GT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TypeMapContext : public TypeReferenceContext {
  public:
    TypeMapContext(TypeReferenceContext *ctx);

    antlr4::tree::TerminalNode *MAP();
    antlr4::tree::TerminalNode *LT();
    std::vector<TypeReferenceContext *> typeReference();
    TypeReferenceContext* typeReference(size_t i);
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *GT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  TypeOptionalContext : public TypeReferenceContext {
  public:
    TypeOptionalContext(TypeReferenceContext *ctx);

    antlr4::tree::TerminalNode *OPTIONAL();
    antlr4::tree::TerminalNode *LT();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *GT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  TypeReferenceContext* typeReference();

  class  QualifiedNameContext : public antlr4::ParserRuleContext {
  public:
    QualifiedNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOUBLE_COLON();
    antlr4::tree::TerminalNode* DOUBLE_COLON(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  QualifiedNameContext* qualifiedName();

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
    std::vector<antlr4::tree::TerminalNode *> NATURAL();
    antlr4::tree::TerminalNode* NATURAL(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    StrategyContext *strategy();
    antlr4::tree::TerminalNode *RPAREN();
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
    std::vector<antlr4::tree::TerminalNode *> ARROW();
    antlr4::tree::TerminalNode* ARROW(size_t i);

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

  class  StratContinueContext : public StrategyContext {
  public:
    StratContinueContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *CONTINUE();

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

