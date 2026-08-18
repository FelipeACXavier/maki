#pragma once

#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "result.h"
#include "symbol.h"
#include "typing/type_reference.h"

namespace koda
{
class SymbolRegistry
{
public:
  struct Scope
  {
    SymbolId owner = InvalidSymbol;
    SymbolId parentOwner = InvalidSymbol;
    std::map<std::string, SymbolId> symbols;
  };

  SymbolRegistry();

  void print() const;
  void clear();

  Result<SymbolId> declare(SymbolKind kind, const std::string& name, const types::TypeReference& type, const Span& span,
                           SymbolId owner = InvalidSymbol);

  std::optional<SymbolId> lookup(const std::string& name, SymbolId owner = InvalidSymbol) const;
  std::optional<SymbolId> lookupLocal(const std::string& name, SymbolId owner = InvalidSymbol) const;
  std::optional<SymbolId> lookupChild(SymbolId owner, const std::string& name) const;

  const Symbol* get(SymbolId id) const;
  Symbol* get(SymbolId id);

  const Scope* scope(SymbolId owner) const;
  std::vector<SymbolId> children(SymbolId owner, SymbolKind kind = SymbolKind::Unknown) const;

  std::optional<SymbolId> component(const std::string& name) const;

private:
  Scope& ensureScope(SymbolId owner);
  SymbolId parentOwner(SymbolId owner) const;

  std::vector<Symbol> mSymbols;
  // Symbols might have scopes, for example, variables might only exist inside functions
  std::unordered_map<SymbolId, Scope> mScopes;
};

}  // namespace koda
