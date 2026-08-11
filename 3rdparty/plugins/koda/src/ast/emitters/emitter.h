#pragma once

#include <string>
#include <vector>

#include "ast/koda_ir.h"
#include "ast/symbol_registry.h"
#include "result.h"

namespace koda
{
struct CompilerOptions;

class Emitter
{
public:
  virtual ~Emitter() = default;

  virtual std::string id() const = 0;
  virtual VoidResult generate(const ir::Program& program, const SymbolRegistry& symbols, const CompilerOptions& options) = 0;
  virtual const std::vector<std::string>& generatedFiles() const = 0;
};

}  // namespace koda
