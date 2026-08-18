#pragma once

#include <cstdint>
#include <format>
#include <limits>
#include <string>

#include "ast.h"
#include "typing/helpers.h"
#include "typing/type_reference.h"
#include "typing/types.h"

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

struct Symbol
{
  SymbolId id = InvalidSymbol;
  SymbolKind kind = SymbolKind::Unknown;
  std::string name;
  types::TypeReference type;
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
