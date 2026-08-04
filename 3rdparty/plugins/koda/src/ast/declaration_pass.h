#pragma once

#include "ast.h"
#include "result.h"
#include "symbol_registry.h"

namespace koda
{
class DeclarationPass
{
public:
  DeclarationPass(SymbolRegistry& symbols)
      : mSymbolRegistry(symbols)
  {
  }

  VoidResult run(const System& system);

private:
  VoidResult declareComponent(const PComponent& component);
  VoidResult declareStatement(const PStatement& statement, SymbolId owner);
  VoidResult declareRosDef(const PRosDef& ros, SymbolId owner);

  SymbolRegistry& mSymbolRegistry;
};

}  // namespace koda
