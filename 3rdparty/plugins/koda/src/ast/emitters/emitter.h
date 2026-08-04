#pragma once

#include <string>
#include <vector>

#include "koda_ir.h"
#include "result.h"
#include "symbol_registry.h"

namespace koda
{
struct CompilerOptions;

class Emitter
{
public:
  virtual ~Emitter() = default;

  virtual std::string id() const = 0;
  virtual VoidResult generate(const ir::Program& program,
                              const SymbolRegistry& symbols,
                              const CompilerOptions& options) = 0;
  virtual const std::vector<std::string>& generatedFiles() const = 0;
};

}  // namespace koda
