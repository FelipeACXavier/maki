#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "ast.h"
#include "symbol.h"

namespace koda::ir
{
enum class ComponentKind
{
  Task,
  Capability
};
enum class ArgumentMode
{
  Plain,
  Requires,
  Provides
};
enum class EventKind
{
  Trigger,
  Return,
  Abort,
  Error,
  In,
  Out
};
enum class HandlerKind
{
  Error,
  Abort,
  Emitter,
  EmitterContinue
};
enum class CallKind
{
  CapabilityTrigger,
  Event
};

struct Expression;
struct Strategy;
struct Handler;

using PExpression = std::shared_ptr<Expression>;
using PStrategy = std::shared_ptr<Strategy>;
using PHandler = std::shared_ptr<Handler>;

struct Argument
{
  SymbolId symbol = InvalidSymbol;
  std::string name;
  Type type;
  ArgumentMode mode = ArgumentMode::Plain;
  Span span;

  static ArgumentMode ModeFromKind(koda::Argument::Kind kind)
  {
    switch (kind)
    {
      case koda::Argument::Kind::Req:
        return ArgumentMode::Requires;
      case koda::Argument::Kind::Pro:
        return ArgumentMode::Provides;
      case koda::Argument::Kind::Plain:
      default:
        return ArgumentMode::Plain;
    }
  }
};

struct Call
{
  CallKind kind = CallKind::Event;
  SymbolId receiver = InvalidSymbol;
  SymbolId target = InvalidSymbol;
  std::vector<PExpression> arguments;
  Span span;
};

struct Expression
{
  struct Literal
  {
    std::string text;
    Type type;
  };

  struct Reference
  {
    SymbolId symbol = InvalidSymbol;
  };

  struct CallExpr
  {
    Call call;
  };

  struct Unary
  {
    std::string op;
    PExpression value;
  };

  struct Binary
  {
    std::string op;
    PExpression lhs;
    PExpression rhs;
  };

  std::variant<Literal, Reference, CallExpr, Unary, Binary> value;
  Type type;
  Span span;
};

struct Handler
{
  HandlerKind kind = HandlerKind::Error;
  std::optional<Call> emitter;
  PStrategy body;
  Span span;
};

struct Strategy
{
  struct Sequence
  {
    std::vector<PStrategy> items;
  };

  struct Join
  {
    std::vector<PStrategy> items;
  };

  struct Either
  {
    std::vector<PStrategy> items;
  };

  struct Let
  {
    SymbolId symbol = InvalidSymbol;
    Call call;
  };

  struct Within
  {
    int seconds = 0;
    PStrategy body;
    PStrategy fallback;
    std::vector<PHandler> handlers;
  };

  struct IfElse
  {
    PExpression condition;
    PStrategy thenBranch;
    PStrategy elseBranch;
  };

  struct Repeat
  {
    int seconds = 0;
    int iterations = 0;
    PStrategy body;
    std::vector<PHandler> handlers;
  };

  struct Guard
  {
    PExpression condition;
  };

  struct End
  {
  };

  struct Continue
  {
  };

  struct FlowRef
  {
    SymbolId flow = InvalidSymbol;
  };

  struct TaskCall
  {
    Call call;
    std::vector<PHandler> handlers;
  };

  std::variant<Sequence, Join, Either, Let, Within, IfElse, Repeat, Guard, End, Continue, FlowRef, TaskCall> value;
  Span span;
};

struct Variable
{
  SymbolId symbol = InvalidSymbol;
  std::string name;
  Type type;
  PExpression initial;
  PExpression fallback;
  Span span;
};

struct Event
{
  SymbolId symbol = InvalidSymbol;
  EventKind kind = EventKind::Trigger;
  std::string name;
  Type type;
  std::vector<Argument> arguments;
  Span span;

  static EventKind fromRosKind(koda::RosDef::Kind kind)
  {
    switch (kind)
    {
      case RosDef::Kind::Return:
        return ir::EventKind::Return;
      case RosDef::Kind::Abort:
        return ir::EventKind::Abort;
      case RosDef::Kind::Error:
        return ir::EventKind::Error;
      case RosDef::Kind::In:
        return ir::EventKind::In;
      case RosDef::Kind::Out:
        return ir::EventKind::Out;
      case RosDef::Kind::Trigger:
      default:
        return ir::EventKind::Trigger;
    }
  }
};

struct Flow
{
  SymbolId symbol = InvalidSymbol;
  std::string name;
  std::vector<Argument> arguments;
  PStrategy strategy;
  Span span;
};

struct Component
{
  SymbolId symbol = InvalidSymbol;
  ComponentKind kind = ComponentKind::Task;
  std::string name;
  std::string title;
  std::string message;
  std::vector<Argument> arguments;
  std::vector<Variable> variables;
  std::vector<Event> events;
  std::vector<Flow> flows;
  Span span;
};

struct Program
{
  std::vector<Component> components;
};

}  // namespace koda::ir
