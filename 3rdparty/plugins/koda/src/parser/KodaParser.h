
// Generated from Koda.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KodaParser : public antlr4::Parser {
public:
  enum {
    TASK = 1, CAPABILITY = 2, TYPE = 3, ENUM = 4, EXTENDS = 5, LIST = 6, 
    OPTIONAL = 7, MAP = 8, MAPPING = 9, TO = 10, ANNOTATIONS = 11, STRATEGY = 12, 
    PARAMETERS = 13, PROPERTIES = 14, ACTION = 15, SERVICE = 16, TOPIC = 17, 
    IMPLIES = 18, WHILE = 19, ALWAYS = 20, EVENTUALLY = 21, NEXT = 22, NEVER = 23, 
    NEGATION = 24, IF = 25, BETWEEN = 26, UNTIL = 27, IS = 28, WAS = 29, 
    RUNNING = 30, STARTED = 31, REJECTED = 32, STOPPED = 33, ABORTED = 34, 
    TRIGGER = 35, RETURN = 36, ABORT = 37, ERROR = 38, IN = 39, OUT = 40, 
    ON = 41, CONSUMES = 42, PRODUCES = 43, DATA = 44, REQ = 45, PRO = 46, 
    SUCCESS = 47, FAILURE = 48, CONTINUE = 49, REPEAT = 50, JOIN = 51, EITHER = 52, 
    WHEN = 53, CHOOSE = 54, WITHIN = 55, DO = 56, ELSE = 57, THEN = 58, 
    EVERY = 59, ARROW = 60, PIPE = 61, EQ = 62, NEQ = 63, LEQ = 64, GEQ = 65, 
    LT = 66, GT = 67, ASSIGN = 68, DOUBLE_COLON = 69, COLON = 70, COMMA = 71, 
    SEMI = 72, DOT = 73, PLUS = 74, MINUS = 75, STAR = 76, SLASH = 77, NOT = 78, 
    AND = 79, OR = 80, LPAREN = 81, RPAREN = 82, LBRACE = 83, RBRACE = 84, 
    LBRACK = 85, RBRACK = 86, NATURAL = 87, REAL = 88, BOOLEAN = 89, IDENT = 90, 
    STRING = 91, ANY = 92, LINE_COMMENT = 93, BLOCK_COMMENT = 94, WS = 95
  };

  enum {
    RuleSystem = 0, RuleTopLevelDeclaration = 1, RuleTopLevelComponent = 2, 
    RuleTypeDeclaration = 3, RuleFieldDeclaration = 4, RuleAnnotationDeclaration = 5, 
    RuleAnnotation = 6, RuleEnumDeclaration = 7, RuleEnumValue = 8, RuleEnumLiteral = 9, 
    RuleMappingDeclaration = 10, RuleTypeReference = 11, RuleQualifiedName = 12, 
    RuleArgumentList = 13, RuleArgument = 14, RuleStatement = 15, RuleTasksBlock = 16, 
    RuleFlow = 17, RuleIdentList = 18, RuleVarsBlock = 19, RuleVariableStatement = 20, 
    RulePropertiesBlock = 21, RulePropertyStatement = 22, RuleActionBlock = 23, 
    RuleServiceBlock = 24, RuleTopicBlock = 25, RulePropertyExpr = 26, RulePropertyHelper = 27, 
    RulePropertyImplication = 28, RulePropertyConditionOr = 29, RulePropertyConditionAnd = 30, 
    RulePropertyConditionUntil = 31, RulePropertyConditionUnary = 32, RulePropertyConsequence = 33, 
    RulePropertyObservation = 34, RulePropertyReference = 35, RuleRosDefStatement = 36, 
    RuleReqDefStatement = 37, RuleEventDefStatement = 38, RuleDataBlock = 39, 
    RuleStrategy = 40, RuleChooseWhenStatement = 41, RuleStrategyHandler = 42, 
    RuleEventStatement = 43, RuleExprList = 44, RuleExpression = 45, RuleExprOr = 46, 
    RuleExprAnd = 47, RuleExprCmp = 48, RuleCompOp = 49, RuleExprNot = 50, 
    RuleExprAdd = 51, RuleExprMul = 52, RuleExprUnary = 53, RuleExprPrimary = 54, 
    RuleRecordLiteral = 55, RuleRecordFieldInitializer = 56, RuleListLiteral = 57, 
    RuleMapLiteral = 58, RuleMapFieldInitializer = 59, RuleIdentifier = 60
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
  class FieldDeclarationContext;
  class AnnotationDeclarationContext;
  class AnnotationContext;
  class EnumDeclarationContext;
  class EnumValueContext;
  class EnumLiteralContext;
  class MappingDeclarationContext;
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
  class PropertiesBlockContext;
  class PropertyStatementContext;
  class ActionBlockContext;
  class ServiceBlockContext;
  class TopicBlockContext;
  class PropertyExprContext;
  class PropertyHelperContext;
  class PropertyImplicationContext;
  class PropertyConditionOrContext;
  class PropertyConditionAndContext;
  class PropertyConditionUntilContext;
  class PropertyConditionUnaryContext;
  class PropertyConsequenceContext;
  class PropertyObservationContext;
  class PropertyReferenceContext;
  class RosDefStatementContext;
  class ReqDefStatementContext;
  class EventDefStatementContext;
  class DataBlockContext;
  class StrategyContext;
  class ChooseWhenStatementContext;
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
  class RecordLiteralContext;
  class RecordFieldInitializerContext;
  class ListLiteralContext;
  class MapLiteralContext;
  class MapFieldInitializerContext;
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
    MappingDeclarationContext *mappingDeclaration();


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

  class  FieldDeclarationContext : public antlr4::ParserRuleContext {
  public:
    FieldDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *COLON();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *SEMI();
    AnnotationDeclarationContext *annotationDeclaration();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FieldDeclarationContext* fieldDeclaration();

  class  AnnotationDeclarationContext : public antlr4::ParserRuleContext {
  public:
    AnnotationDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ANNOTATIONS();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<AnnotationContext *> annotation();
    AnnotationContext* annotation(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AnnotationDeclarationContext* annotationDeclaration();

  class  AnnotationContext : public antlr4::ParserRuleContext {
  public:
    AnnotationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *COLON();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AnnotationContext* annotation();

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

  class  MappingDeclarationContext : public antlr4::ParserRuleContext {
  public:
    MappingDeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MAPPING();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *TO();
    antlr4::tree::TerminalNode *STRING();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MappingDeclarationContext* mappingDeclaration();

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

    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *IDENT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArgReqContext : public ArgumentContext {
  public:
    ArgReqContext(ArgumentContext *ctx);

    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *REQ();
    TypeReferenceContext *typeReference();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ArgProContext : public ArgumentContext {
  public:
    ArgProContext(ArgumentContext *ctx);

    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *PRO();
    TypeReferenceContext *typeReference();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ArgumentContext* argument();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TasksBlockContext *tasksBlock();
    VarsBlockContext *varsBlock();
    PropertiesBlockContext *propertiesBlock();
    ActionBlockContext *actionBlock();
    ServiceBlockContext *serviceBlock();
    TopicBlockContext *topicBlock();
    RosDefStatementContext *rosDefStatement();
    DataBlockContext *dataBlock();


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
    antlr4::tree::TerminalNode *PARAMETERS();
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
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *ASSIGN();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VariableStatementContext* variableStatement();

  class  PropertiesBlockContext : public antlr4::ParserRuleContext {
  public:
    PropertiesBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PROPERTIES();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<PropertyStatementContext *> propertyStatement();
    PropertyStatementContext* propertyStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertiesBlockContext* propertiesBlock();

  class  PropertyStatementContext : public antlr4::ParserRuleContext {
  public:
    PropertyStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *COLON();
    PropertyExprContext *propertyExpr();
    antlr4::tree::TerminalNode *SEMI();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyStatementContext* propertyStatement();

  class  ActionBlockContext : public antlr4::ParserRuleContext {
  public:
    ActionBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ACTION();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<ReqDefStatementContext *> reqDefStatement();
    ReqDefStatementContext* reqDefStatement(size_t i);
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
    std::vector<ReqDefStatementContext *> reqDefStatement();
    ReqDefStatementContext* reqDefStatement(size_t i);
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
    std::vector<ReqDefStatementContext *> reqDefStatement();
    ReqDefStatementContext* reqDefStatement(size_t i);
    std::vector<RosDefStatementContext *> rosDefStatement();
    RosDefStatementContext* rosDefStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopicBlockContext* topicBlock();

  class  PropertyExprContext : public antlr4::ParserRuleContext {
  public:
    PropertyExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyHelperContext *propertyHelper();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyExprContext* propertyExpr();

  class  PropertyHelperContext : public antlr4::ParserRuleContext {
  public:
    PropertyHelperContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IF();
    std::vector<PropertyConditionOrContext *> propertyConditionOr();
    PropertyConditionOrContext* propertyConditionOr(size_t i);
    PropertyConsequenceContext *propertyConsequence();
    antlr4::tree::TerminalNode *BETWEEN();
    antlr4::tree::TerminalNode *AND();
    PropertyImplicationContext *propertyImplication();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyHelperContext* propertyHelper();

  class  PropertyImplicationContext : public antlr4::ParserRuleContext {
  public:
    PropertyImplicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyConditionOrContext *propertyConditionOr();
    antlr4::tree::TerminalNode *IMPLIES();
    PropertyImplicationContext *propertyImplication();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyImplicationContext* propertyImplication();

  class  PropertyConditionOrContext : public antlr4::ParserRuleContext {
  public:
    PropertyConditionOrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyConditionAndContext *propertyConditionAnd();
    antlr4::tree::TerminalNode *OR();
    PropertyConditionOrContext *propertyConditionOr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyConditionOrContext* propertyConditionOr();

  class  PropertyConditionAndContext : public antlr4::ParserRuleContext {
  public:
    PropertyConditionAndContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyConditionUntilContext *propertyConditionUntil();
    PropertyConditionAndContext *propertyConditionAnd();
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *WHILE();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyConditionAndContext* propertyConditionAnd();

  class  PropertyConditionUntilContext : public antlr4::ParserRuleContext {
  public:
    PropertyConditionUntilContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<PropertyConditionUnaryContext *> propertyConditionUnary();
    PropertyConditionUnaryContext* propertyConditionUnary(size_t i);
    antlr4::tree::TerminalNode *UNTIL();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyConditionUntilContext* propertyConditionUntil();

  class  PropertyConditionUnaryContext : public antlr4::ParserRuleContext {
  public:
    PropertyConditionUnaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NEGATION();
    PropertyExprContext *propertyExpr();
    antlr4::tree::TerminalNode *ALWAYS();
    antlr4::tree::TerminalNode *EVENTUALLY();
    antlr4::tree::TerminalNode *NEVER();
    antlr4::tree::TerminalNode *NEXT();
    PropertyObservationContext *propertyObservation();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyConditionUnaryContext* propertyConditionUnary();

  class  PropertyConsequenceContext : public antlr4::ParserRuleContext {
  public:
    PropertyConsequenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALWAYS();
    PropertyExprContext *propertyExpr();
    antlr4::tree::TerminalNode *EVENTUALLY();
    antlr4::tree::TerminalNode *NEVER();
    antlr4::tree::TerminalNode *NEXT();
    antlr4::tree::TerminalNode *BETWEEN();
    std::vector<PropertyConditionOrContext *> propertyConditionOr();
    PropertyConditionOrContext* propertyConditionOr(size_t i);
    antlr4::tree::TerminalNode *AND();
    PropertyConsequenceContext *propertyConsequence();
    PropertyImplicationContext *propertyImplication();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyConsequenceContext* propertyConsequence();

  class  PropertyObservationContext : public antlr4::ParserRuleContext {
  public:
    PropertyObservationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyReferenceContext *propertyReference();
    antlr4::tree::TerminalNode *IS();
    antlr4::tree::TerminalNode *RUNNING();
    antlr4::tree::TerminalNode *STARTED();
    antlr4::tree::TerminalNode *WAS();
    antlr4::tree::TerminalNode *REJECTED();
    antlr4::tree::TerminalNode *STOPPED();
    antlr4::tree::TerminalNode *ABORTED();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyObservationContext* propertyObservation();

  class  PropertyReferenceContext : public antlr4::ParserRuleContext {
  public:
    PropertyReferenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *DOT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyReferenceContext* propertyReference();

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

  class  ReqDefStatementContext : public antlr4::ParserRuleContext {
  public:
    ReqDefStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONSUMES();
    antlr4::tree::TerminalNode *COLON();
    TypeReferenceContext *typeReference();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *PRODUCES();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReqDefStatementContext* reqDefStatement();

  class  EventDefStatementContext : public antlr4::ParserRuleContext {
  public:
    EventDefStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    ArgumentListContext *argumentList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EventDefStatementContext* eventDefStatement();

  class  DataBlockContext : public antlr4::ParserRuleContext {
  public:
    DataBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DATA();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<VariableStatementContext *> variableStatement();
    VariableStatementContext* variableStatement(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DataBlockContext* dataBlock();

  class  StrategyContext : public antlr4::ParserRuleContext {
  public:
    StrategyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    StrategyContext() = default;
    void copyFrom(StrategyContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
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

  class  StratSuccessContext : public StrategyContext {
  public:
    StratSuccessContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *SUCCESS();

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

  class  StratFailureContext : public StrategyContext {
  public:
    StratFailureContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *FAILURE();

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

  class  StratContinueContext : public StrategyContext {
  public:
    StratContinueContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *CONTINUE();

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

  class  StratChooseContext : public StrategyContext {
  public:
    StratChooseContext(StrategyContext *ctx);

    antlr4::tree::TerminalNode *CHOOSE();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<ChooseWhenStatementContext *> chooseWhenStatement();
    ChooseWhenStatementContext* chooseWhenStatement(size_t i);

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  StrategyContext* strategy();
  StrategyContext* strategy(int precedence);
  class  ChooseWhenStatementContext : public antlr4::ParserRuleContext {
  public:
    ChooseWhenStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ChooseWhenStatementContext() = default;
    void copyFrom(ChooseWhenStatementContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  WhenStatementContext : public ChooseWhenStatementContext {
  public:
    WhenStatementContext(ChooseWhenStatementContext *ctx);

    antlr4::tree::TerminalNode *WHEN();
    antlr4::tree::TerminalNode *COLON();
    StrategyContext *strategy();
    antlr4::tree::TerminalNode *SEMI();
    ExpressionContext *expression();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ChooseWhenStatementContext* chooseWhenStatement();

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

  class  ExprBooleanContext : public ExprPrimaryContext {
  public:
    ExprBooleanContext(ExprPrimaryContext *ctx);

    antlr4::tree::TerminalNode *BOOLEAN();

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

  class  ExprMapLiteralContext : public ExprPrimaryContext {
  public:
    ExprMapLiteralContext(ExprPrimaryContext *ctx);

    MapLiteralContext *mapLiteral();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprRecordContext : public ExprPrimaryContext {
  public:
    ExprRecordContext(ExprPrimaryContext *ctx);

    RecordLiteralContext *recordLiteral();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExprListLiteralContext : public ExprPrimaryContext {
  public:
    ExprListLiteralContext(ExprPrimaryContext *ctx);

    ListLiteralContext *listLiteral();

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

  class  ExprDataAccessContext : public ExprPrimaryContext {
  public:
    ExprDataAccessContext(ExprPrimaryContext *ctx);

    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *DOT();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  ExprPrimaryContext* exprPrimary();

  class  RecordLiteralContext : public antlr4::ParserRuleContext {
  public:
    RecordLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<RecordFieldInitializerContext *> recordFieldInitializer();
    RecordFieldInitializerContext* recordFieldInitializer(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RecordLiteralContext* recordLiteral();

  class  RecordFieldInitializerContext : public antlr4::ParserRuleContext {
  public:
    RecordFieldInitializerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *COLON();
    ExpressionContext *expression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RecordFieldInitializerContext* recordFieldInitializer();

  class  ListLiteralContext : public antlr4::ParserRuleContext {
  public:
    ListLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACK();
    antlr4::tree::TerminalNode *RBRACK();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ListLiteralContext* listLiteral();

  class  MapLiteralContext : public antlr4::ParserRuleContext {
  public:
    MapLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    std::vector<MapFieldInitializerContext *> mapFieldInitializer();
    MapFieldInitializerContext* mapFieldInitializer(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MapLiteralContext* mapLiteral();

  class  MapFieldInitializerContext : public antlr4::ParserRuleContext {
  public:
    MapFieldInitializerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *GT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MapFieldInitializerContext* mapFieldInitializer();

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
    antlr4::tree::TerminalNode *CONSUMES();
    antlr4::tree::TerminalNode *PRODUCES();


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

