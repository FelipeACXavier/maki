#pragma once

#include <vector>

#include "ast/emitters/emitter.h"
#include "tracer_writer.h"

namespace koda
{
class TracerEmitter final : public Emitter
{
public:
  std::string id() const override;

  VoidResult generate(const ir::Program& program, const SymbolRegistry& symbols, const koda::types::TypeRegistry& registry,
                      const CompilerOptions& options) override;

  const std::vector<std::string>& generatedFiles() const override;

private:
  tracer::Writer mWriter;
  std::vector<std::string> mGeneratedFiles;
};

}  // namespace koda
