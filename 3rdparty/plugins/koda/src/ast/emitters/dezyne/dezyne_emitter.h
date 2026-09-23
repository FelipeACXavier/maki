#pragma once

#include <string>
#include <vector>

#include "ast/emitters/emitter.h"
#include "dezyne_model.h"
#include "dezyne_writer.h"

namespace koda
{
class DezyneEmitter final : public Emitter
{
public:
  std::string id() const override;
  VoidResult generate(const ir::Program& program, const SymbolRegistry& symbols, const koda::types::TypeRegistry& registry,
                      const CompilerOptions& options) override;
  const std::vector<std::string>& generatedFiles() const override;
  const dezyne::Model& model() const;

private:
  dezyne::Model mModel;
  dezyne::Writer mWriter;
  std::vector<std::string> mGeneratedFiles;
};

}  // namespace koda
