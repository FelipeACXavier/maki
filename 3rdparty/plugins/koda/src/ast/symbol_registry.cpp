#include "symbol_registry.h"

#include <format>

#include "logging.h"

namespace koda
{
SymbolRegistry::SymbolRegistry()
{
  clear();
}

void SymbolRegistry::clear()
{
  mSymbols.clear();
  mScopes.clear();
  mScopes.emplace(InvalidSymbol, Scope{InvalidSymbol, InvalidSymbol, {}});
}

void SymbolRegistry::print()
{
  LOG_DEBUG("Symbol register:");
  for (const auto& s : mSymbols)
  {
    LOG_DEBUG("  Symbol: %u %s", s.id, s.name.c_str());
    LOG_DEBUG("    Kind: %s", toString(s.kind).c_str());
    LOG_DEBUG("    Type: %s", s.type.toString().c_str());
    LOG_DEBUG("    Span: %s", s.span.toString().c_str());
    LOG_DEBUG("    Owner: %u", s.owner);
  }
}

Result<SymbolId> SymbolRegistry::declare(SymbolKind kind, const std::string& name, const Type& type, const Span& span, SymbolId owner)
{
  auto& s = ensureScope(owner);
  if (s.symbols.contains(name))
  {
    const auto previous = s.symbols.at(name);
    const auto* previousSymbol = get(previous);
    return Result<SymbolId>::Failed(
        std::format("Duplicate {} '{}'; previous declaration at {}", toString(kind), name, previousSymbol ? previousSymbol->span.toString() : "unknown location"));
  }

  const auto id = static_cast<SymbolId>(mSymbols.size());
  mSymbols.push_back(Symbol{id, kind, name, type, span, owner});
  s.symbols.emplace(name, id);

  if (kind == SymbolKind::Task || kind == SymbolKind::Capability)
    ensureScope(id);

  return id;
}

std::optional<SymbolId> SymbolRegistry::lookupLocal(const std::string& name, SymbolId owner) const
{
  auto it = mScopes.find(owner);
  if (it == mScopes.end())
    return std::nullopt;
  auto symbolIt = it->second.symbols.find(name);
  if (symbolIt == it->second.symbols.end())
    return std::nullopt;
  return symbolIt->second;
}

std::optional<SymbolId> SymbolRegistry::lookup(const std::string& name, SymbolId owner) const
{
  auto current = owner;
  while (true)
  {
    if (auto found = lookupLocal(name, current))
      return found;
    if (current == InvalidSymbol)
      break;
    current = parentOwner(current);
  }
  return std::nullopt;
}

std::optional<SymbolId> SymbolRegistry::lookupChild(SymbolId owner, const std::string& name) const
{
  return lookupLocal(name, owner);
}

const Symbol* SymbolRegistry::get(SymbolId id) const
{
  if (id == InvalidSymbol || id >= mSymbols.size())
    return nullptr;

  return &mSymbols[id];
}

Symbol* SymbolRegistry::get(SymbolId id)
{
  if (id == InvalidSymbol || id >= mSymbols.size())
    return nullptr;

  return &mSymbols[id];
}

const SymbolRegistry::Scope* SymbolRegistry::scope(SymbolId owner) const
{
  auto it = mScopes.find(owner);
  return it == mScopes.end() ? nullptr : &it->second;
}

std::vector<SymbolId> SymbolRegistry::children(SymbolId owner, SymbolKind kind) const
{
  std::vector<SymbolId> result;
  const auto* s = scope(owner);
  if (!s)
    return result;
  for (const auto& [_, id] : s->symbols)
  {
    const auto* symbol = get(id);
    if (symbol && (kind == SymbolKind::Unknown || symbol->kind == kind))
      result.push_back(id);
  }
  return result;
}

std::optional<SymbolId> SymbolRegistry::component(const std::string& name) const
{
  auto id = lookupLocal(name, InvalidSymbol);
  if (!id)
    return std::nullopt;
  const auto* symbol = get(*id);
  if (!symbol || (symbol->kind != SymbolKind::Task && symbol->kind != SymbolKind::Capability))
    return std::nullopt;
  return id;
}

std::optional<Type> SymbolRegistry::resolveType(const std::string& name) const
{
  if (name == "int" || name == "integer")
    return Type::Int();
  if (name == "float" || name == "real" || name == "double")
    return Type::Float();
  if (name == "string" || name == "str")
    return Type::String();
  if (name == "bool" || name == "boolean")
    return Type::Bool();

  if (auto c = component(name))
    return Type::Component(name, *c);

  return Type::Custom(name);
}

SymbolRegistry::Scope& SymbolRegistry::ensureScope(SymbolId owner)
{
  auto [it, _] = mScopes.try_emplace(owner, Scope{owner, parentOwner(owner), {}});
  return it->second;
}

SymbolId SymbolRegistry::parentOwner(SymbolId owner) const
{
  if (owner == InvalidSymbol)
    return InvalidSymbol;

  const auto* symbol = get(owner);
  return symbol ? symbol->owner : InvalidSymbol;
}

}  // namespace koda
