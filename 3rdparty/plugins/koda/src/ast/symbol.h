#pragma once

#include <cstdint>
#include <format>
#include <limits>
#include <string>

#include "ast.h"

namespace koda
{
using SymbolId = std::uint32_t;
inline constexpr SymbolId InvalidSymbol = std::numeric_limits<SymbolId>::max();

enum class SymbolKind
{
  Unknown = 0,
  Task,
  Capability,
  Argument,
  Variable,
  Flow,
  Event,
  Local
};

enum class TypeKind
{
  Unknown = 0,
  Void,
  Bool,
  Int,
  Float,
  String,
  Component,
  Custom
};

struct Type
{
  TypeKind kind = TypeKind::Unknown;
  std::string name;
  SymbolId symbol = InvalidSymbol;

  bool valid() const
  {
    return kind != TypeKind::Unknown;
  }

  bool isNumeric() const
  {
    return kind == TypeKind::Int || kind == TypeKind::Float;
  }

  std::string toString() const
  {
    if (kind == TypeKind::Void)
      return "Void";
    if (kind == TypeKind::Bool)
      return "Bool";
    if (kind == TypeKind::Int)
      return "Int";
    if (kind == TypeKind::Float)
      return "Float";
    if (kind == TypeKind::String)
      return "String";
    if (kind == TypeKind::Component)
      return std::format("Component ({})", name);
    if (kind == TypeKind::Custom)
      return std::format("Custom ({})", name);

    return "Unknown";
  }

  static Type Unknown()
  {
    return {};
  }

  static Type Void()
  {
    return {TypeKind::Void, "void", InvalidSymbol};
  }

  static Type Bool()
  {
    return {TypeKind::Bool, "bool", InvalidSymbol};
  }

  static Type Int()
  {
    return {TypeKind::Int, "int", InvalidSymbol};
  }

  static Type Float()
  {
    return {TypeKind::Float, "float", InvalidSymbol};
  }

  static Type String()
  {
    return {TypeKind::String, "string", InvalidSymbol};
  }

  static Type Component(std::string name, SymbolId symbol)
  {
    return {TypeKind::Component, std::move(name), symbol};
  }

  static Type Custom(std::string name)
  {
    return {TypeKind::Custom, std::move(name), InvalidSymbol};
  }
};

struct Symbol
{
  SymbolId id = InvalidSymbol;
  SymbolKind kind = SymbolKind::Unknown;
  std::string name;
  Type type;
  Span span;
  SymbolId owner = InvalidSymbol;
};

inline const std::string toString(SymbolKind kind)
{
  switch (kind)
  {
    case SymbolKind::Task:
      return "task";
    case SymbolKind::Capability:
      return "capability";
    case SymbolKind::Argument:
      return "argument";
    case SymbolKind::Variable:
      return "variable";
    case SymbolKind::Flow:
      return "flow";
    case SymbolKind::Event:
      return "event";
    case SymbolKind::Local:
      return "local";
    default:
      return "symbol";
  }
}

typedef std::optional<SymbolId> OSymbolId;
typedef std::optional<Symbol> OSymbol;
inline constexpr OSymbolId NullSymbolId = std::nullopt;
inline constexpr OSymbol NullSymbol = std::nullopt;
}  // namespace koda
