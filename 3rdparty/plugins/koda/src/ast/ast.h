// KodaAst.h
#pragma once
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace koda
{
struct Component;
struct Statement;
struct Argument;

struct Flow;
struct VarDef;
struct RosDef;     // trigger/return/abort/error/in/out blocks
struct ActionDef;  // action/service/topic blocks
struct VarsBlock;
struct StrategyBlock;

struct Strategy;   // fwd
struct Expr;       // fwd
struct EventCall;  // fwd

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
  std::string srcId;
  std::vector<std::shared_ptr<Argument>> args;
  std::vector<std::shared_ptr<Statement>> statements;
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
  
  std::string kindToString() const;
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
  std::string a;
  std::string b;
  std::string srcId;
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
  
  std::string aKindToString() const;
};

// ---------- Statements ----------
struct Statement
{
  std::variant<
      std::shared_ptr<StrategyBlock>,
      std::shared_ptr<VarsBlock>,
      std::shared_ptr<RosDef>,
      std::shared_ptr<ActionDef>>
      node;
  Span span;

  void print(const std::string& prefix, const bool last) const;
  
};

struct Flow
{
  std::string name;
  std::string srcId;
  std::vector<std::string> tags;
  std::shared_ptr<Strategy> strategy;
  Span span;

  void print(const std::string& prefix, const bool last) const;
  
};

struct VarDef
{
  std::string varType;
  std::string name;
  std::string srcId;
  std::shared_ptr<Expr> init;
  std::shared_ptr<Expr> fallback;
  Span span;

  void print(const std::string& prefix, const bool last) const;
  
};

// ---------- ROS defs ----------
struct EventDefComponent
{
  // Keep it simple for now; you can structure this later similarly to Strategy/Expr.
  std::string kind;  // "ros_event", "timeout", ...
  std::string text;  // raw (or structured fields later)

  void print(const std::string& prefix, const bool last) const;
  
};

struct EventDef
{
  std::string typeName;
  std::string name;
  std::string srcId;
  std::vector<std::shared_ptr<Argument>> args;
  std::vector<std::shared_ptr<EventDefComponent>> components;
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
  std::string srcId;
  // Rascal had `call` marker in grammar; we mapped it as CALL token.
  // You can store whether it's "call" or some other modifier if you later expand it.
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
  
  std::string rKindToString() const;
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
  std::string srcId;
  std::vector<std::shared_ptr<RosDef>> rosDefs;
  Span span;

  std::string toString() const;
  void print(const std::string& prefix, const bool last) const;
  
  std::string adKindToString() const;
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
  std::string srcId;
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
  struct Let
  {
    std::string name;
    std::shared_ptr<EventCall> call;

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
  struct IfElse
  {
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Strategy> a;
    std::shared_ptr<Strategy> b;

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
  struct Guard
  {
    std::shared_ptr<Expr> cond;

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
  struct Ref
  {
    std::string name;
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

  std::variant<
      std::shared_ptr<Seq>,
      std::shared_ptr<Join>,
      std::shared_ptr<Either>,
      std::shared_ptr<Let>,
      std::shared_ptr<Within>,
      std::shared_ptr<IfElse>,
      std::shared_ptr<Repeat>,
      std::shared_ptr<Guard>,
      std::shared_ptr<End>,
      std::shared_ptr<Continue>,
      std::shared_ptr<Ref>,
      std::shared_ptr<TaskCall>,
      std::shared_ptr<Paren>>
      v;

  std::string srcId;
  void print(const std::string& prefix, const bool last) const;
  Span span;
  
};

// ---------- EventCall ----------
struct EventCall
{
  // qualified: a.b(args)  OR  unqualified: a(args)
  std::string receiver;  // a
  std::string name;      // b or a
  std::string srcId;
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

  std::variant<
      std::shared_ptr<Id>,
      std::shared_ptr<Str>,
      std::shared_ptr<Int>,
      std::shared_ptr<Float>,
      std::shared_ptr<Call>,
      std::shared_ptr<Neg>,
      std::shared_ptr<Not>,
      std::shared_ptr<BinOp>,
      std::shared_ptr<Paren>>
      v;

  std::string srcId;
  Span span;

  void print(const std::string& prefix, const bool last) const;
  
};

typedef std::shared_ptr<Component> PComponent;
typedef std::shared_ptr<Argument> PArgument;
typedef std::shared_ptr<Statement> PStatement;
typedef std::shared_ptr<StrategyBlock> PStrategyBlock;
typedef std::shared_ptr<VarsBlock> PVarsBlock;
typedef std::shared_ptr<RosDef> PRosDef;
typedef std::shared_ptr<ActionDef> PActionDef;
typedef std::shared_ptr<VarDef> PVarDef;
typedef std::shared_ptr<Flow> PFlow;
typedef std::shared_ptr<Strategy> PStrategy;
typedef std::shared_ptr<EventDefComponent> PEventDefComponent;
typedef std::shared_ptr<EventDef> PEventDef;
typedef std::shared_ptr<EventCall> PEventCall;
typedef std::shared_ptr<Strategy::Seq> PSeq;
typedef std::shared_ptr<Strategy::Join> PJoin;
typedef std::shared_ptr<Strategy::Either> PEither;
typedef std::shared_ptr<Strategy::Let> PLet;
typedef std::shared_ptr<Strategy::Within> PWithin;
typedef std::shared_ptr<Strategy::IfElse> PIfElse;
typedef std::shared_ptr<Strategy::Repeat> PRepeat;
typedef std::shared_ptr<Strategy::Guard> PGuard;
typedef std::shared_ptr<Strategy::End> PEnd;
typedef std::shared_ptr<Strategy::Continue> PContinue;
typedef std::shared_ptr<Strategy::Ref> PRef;
typedef std::shared_ptr<Strategy::TaskCall> PTaskCall;
typedef std::shared_ptr<Strategy::Paren> PParen;
typedef std::shared_ptr<StrategyHandler> PStrategyHandler;

typedef std::shared_ptr<Expr> PExpr;
typedef std::shared_ptr<Expr::Id> PId;
typedef std::shared_ptr<Expr::Str> PStr;
typedef std::shared_ptr<Expr::Int> PInt;
typedef std::shared_ptr<Expr::Float> PFloat;
typedef std::shared_ptr<Expr::Call> PCall;
typedef std::shared_ptr<Expr::Neg> PNeg;
typedef std::shared_ptr<Expr::Not> PNot;
typedef std::shared_ptr<Expr::BinOp> PBinOp;
typedef std::shared_ptr<Expr::Paren> PEParen;

}  // namespace koda