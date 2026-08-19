#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "typing/type_reference.h"

namespace koda
{
struct Component;
struct TypeMapping;
struct Statement;
struct Argument;

struct Flow;
struct VarDef;
struct RosDef;
struct ActionDef;
struct VarsBlock;
struct StrategyBlock;

struct Strategy;
struct Expr;
struct EventCall;

extern bool gPrintSpan;

// ---------- Common ----------
struct Span
{
  int lineStart = 0, colStart = 0;
  int lineEnd = 0, colEnd = 0;

  std::string toString() const;
};

// ---------- Top-level ----------
struct System
{
  std::vector<std::shared_ptr<TypeMapping>> mappings;
  std::vector<std::shared_ptr<Component>> components;
  void print() const;
};

struct Component
{
  enum class Kind
  {
    Unknown = 0,
    Task,
    Capability
  };
  Kind kind;
  std::string name;
  std::vector<std::shared_ptr<Argument>> args;
  std::vector<std::shared_ptr<Statement>> statements;
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
};

struct Argument
{
  enum class Kind
  {
    Unknown = 0,
    Plain,
    Req,
    Pro
  };

  Kind kind = Kind::Plain;
  types::TypeReference a;
  std::string b;
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
};

struct TypeMapping
{
  koda::types::TypeReference source;
  std::string destination;

  Span span;
  void print(const std::string& prefix, const bool last) const;
};

// ---------- Statements ----------
struct Statement
{
  std::variant<std::shared_ptr<StrategyBlock>, std::shared_ptr<VarsBlock>, std::shared_ptr<RosDef>, std::shared_ptr<ActionDef>> node;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

struct Flow
{
  std::string name;
  std::vector<std::shared_ptr<Argument>> args;
  std::shared_ptr<Strategy> strategy;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

struct VarDef
{
  types::TypeReference varType;
  std::string name;
  std::shared_ptr<Expr> init;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

// ---------- ROS defs ----------
struct EventDef
{
  std::string typeName;
  std::string name;
  std::vector<std::shared_ptr<Argument>> args;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

struct RosDef
{
  enum class Kind
  {
    Unknown = 0,
    Trigger,
    Return,
    Abort,
    Error,
    In,
    Out
  };
  Kind kind;
  std::shared_ptr<EventDef> def;
  // Rascal had `call` marker in grammar; we mapped it as CALL token.
  // You can store whether it's "call" or some other modifier if you later expand it.
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
};

struct ActionDef
{
  enum class Kind
  {
    Unknown = 0,
    Action,
    Service,
    Topic
  };

  Kind kind;
  std::string label1;  // String
  std::string label2;  // String
  std::vector<std::shared_ptr<RosDef>> rosDefs;
  Span span;

  koda::types::TypeReference consumes;
  koda::types::TypeReference produces;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
};

struct StrategyBlock
{
  std::vector<std::shared_ptr<Flow>> flows;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

struct VarsBlock
{
  std::vector<std::shared_ptr<VarDef>> vars;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

// ---------- Strategy ----------
struct StrategyHandler
{
  enum class Kind
  {
    Unknown = 0,
    OnError,
    OnAbort,
    OnEmitter,
    OnEmitterContinue
  };
  Kind kind;
  std::shared_ptr<EventCall> emitter;  // only for OnEmitter
  std::shared_ptr<Strategy> body;      // points to owned Strategy in container (see builder approach)
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
};

struct Strategy
{
  struct Seq
  {
    std::vector<std::shared_ptr<Strategy>> alts;

    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Join
  {
    std::vector<std::shared_ptr<Strategy>> alts;

    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Either
  {
    std::vector<std::shared_ptr<Strategy>> alts;

    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Within
  {
    int seconds;
    std::shared_ptr<Strategy> a;
    std::shared_ptr<Strategy> b;
    std::vector<std::shared_ptr<StrategyHandler>> handlers;

    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Repeat
  {
    std::shared_ptr<Strategy> a;
    int seconds;
    int iterations;
    std::vector<std::shared_ptr<StrategyHandler>> handlers;

    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct End
  {
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Continue
  {
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct TaskCall
  {
    std::shared_ptr<EventCall> call;
    std::vector<std::shared_ptr<StrategyHandler>> handlers;

    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Paren
  {
    std::shared_ptr<Strategy> a;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  std::variant<std::shared_ptr<Seq>, std::shared_ptr<Join>, std::shared_ptr<Either>, std::shared_ptr<Within>, std::shared_ptr<Repeat>,
               std::shared_ptr<End>, std::shared_ptr<Continue>, std::shared_ptr<TaskCall>, std::shared_ptr<Paren>>
      v;

  void print(const std::string& prefix, const bool last) const;
  Span span;
};

// ---------- EventCall ----------
struct EventCall
{
  // qualified: received.name(args)  OR  unqualified: receiver(args)
  std::string id;
  std::string receiver;
  std::string name;
  std::vector<std::shared_ptr<Expr>> args;
  Span span;

  void print(const std::string& prefix, const bool last) const;
};

// ---------- Expressions ----------
struct Expr
{
  struct Id
  {
    std::string value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Str
  {
    std::string value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Int
  {
    int value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Float
  {
    double value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Bool
  {
    bool value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Call
  {
    std::shared_ptr<EventCall> value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Neg
  {
    std::shared_ptr<Expr> value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Not
  {
    std::shared_ptr<Expr> value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct BinOp
  {
    enum class Kind
    {
      Unknown = 0,
      Equal,
      NotEqual,
      GreaterThan,
      GreaterEqual,
      LessThan,
      LessEqual,
      Addition,
      Subtraction,
      Multiplication,
      Division,
      Negation,
      Unary,
      Disjunction,
      Conjunction
    };
    Kind operation;
    std::shared_ptr<Expr> a;
    std::shared_ptr<Expr> b;

    std::string toString() const;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct Paren
  {
    std::shared_ptr<Expr> value;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  struct RecordLiteral
  {
    struct Field
    {
      std::string name;
      std::shared_ptr<Expr> value;
      Span span;

      void print(const std::string& prefix, const bool last) const;
    };

    std::vector<std::shared_ptr<Field>> fields;
    void print(const std::string& prefix, const bool last, const Span& span) const;
  };

  std::variant<std::shared_ptr<Id>, std::shared_ptr<Str>, std::shared_ptr<Int>, std::shared_ptr<Float>, std::shared_ptr<Bool>, std::shared_ptr<Call>,
               std::shared_ptr<Neg>, std::shared_ptr<Not>, std::shared_ptr<BinOp>, std::shared_ptr<Paren>, std::shared_ptr<RecordLiteral>>
      v;

  Span span;

  void print(const std::string& prefix, const bool last) const;
};

typedef std::shared_ptr<Component> PComponent;
typedef std::shared_ptr<Argument> PArgument;
typedef std::shared_ptr<TypeMapping> PTypeMapping;
typedef std::shared_ptr<Statement> PStatement;
typedef std::shared_ptr<StrategyBlock> PStrategyBlock;
typedef std::shared_ptr<VarsBlock> PVarsBlock;
typedef std::shared_ptr<RosDef> PRosDef;
typedef std::shared_ptr<ActionDef> PActionDef;
typedef std::shared_ptr<VarDef> PVarDef;
typedef std::shared_ptr<Flow> PFlow;
typedef std::shared_ptr<Strategy> PStrategy;
typedef std::shared_ptr<EventDef> PEventDef;
typedef std::shared_ptr<EventCall> PEventCall;
typedef std::shared_ptr<Strategy::Seq> PSeq;
typedef std::shared_ptr<Strategy::Join> PJoin;
typedef std::shared_ptr<Strategy::Either> PEither;
typedef std::shared_ptr<Strategy::Within> PWithin;
typedef std::shared_ptr<Strategy::Repeat> PRepeat;
typedef std::shared_ptr<Strategy::End> PEnd;
typedef std::shared_ptr<Strategy::Continue> PContinue;
typedef std::shared_ptr<Strategy::TaskCall> PTaskCall;
typedef std::shared_ptr<Strategy::Paren> PParen;
typedef std::shared_ptr<StrategyHandler> PStrategyHandler;

typedef std::shared_ptr<Expr> PExpr;
typedef std::shared_ptr<Expr::Id> PId;
typedef std::shared_ptr<Expr::Str> PStr;
typedef std::shared_ptr<Expr::Int> PInt;
typedef std::shared_ptr<Expr::Float> PFloat;
typedef std::shared_ptr<Expr::Bool> PBool;
typedef std::shared_ptr<Expr::Call> PCall;
typedef std::shared_ptr<Expr::Neg> PNeg;
typedef std::shared_ptr<Expr::Not> PNot;
typedef std::shared_ptr<Expr::BinOp> PBinOp;
typedef std::shared_ptr<Expr::Paren> PEParen;
typedef std::shared_ptr<Expr::RecordLiteral> PRecordLiteral;
typedef std::shared_ptr<Expr::RecordLiteral::Field> PRecordLiteralField;

}  // namespace koda
