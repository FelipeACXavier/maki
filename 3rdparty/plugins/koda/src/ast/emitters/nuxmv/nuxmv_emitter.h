#pragma once

#include <vector>

#include "ast/emitters/emitter.h"
#include "nuxmv_model.h"
#include "nuxmv_writer.h"

namespace koda
{
class NuXmvEmitter final : public Emitter
{
public:
  std::string id() const override;

  VoidResult generate(const ir::Program& program, const SymbolRegistry& symbols, const koda::types::TypeRegistry& registry,
                      const CompilerOptions& options) override;

  const std::vector<std::string>& generatedFiles() const override;
  const std::vector<nuxmv::Model>& models() const;

private:
  std::vector<nuxmv::Model> mModels;
  nuxmv::Writer mWriter;
  std::vector<std::string> mGeneratedFiles;
};

}  // namespace koda
