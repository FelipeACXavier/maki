#pragma once

#include "ast.h"
#include "result.h"
#include "symbol_registry.h"
#include "typing/type_registry.h"

namespace koda
{
class DeclarationPass
{
public:
  DeclarationPass(SymbolRegistry& symbols, types::TypeRegistry& types);

  VoidResult run(const System& system);

private:
  VoidResult declareComponent(const PComponent& component);
  VoidResult declareStatement(const PStatement& statement, SymbolId owner);
  VoidResult declareRosDef(const PRosDef& ros, SymbolId owner);

  SymbolRegistry& mSymbolRegistry;
  types::TypeRegistry& mTypeRegistry;
};

}  // namespace koda
