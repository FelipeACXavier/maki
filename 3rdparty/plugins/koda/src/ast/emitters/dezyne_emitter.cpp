#include "dezyne_emitter.h"

#include "ast/koda_compiler.h"
#include "dezyne_declaration_pass.h"
#include "dezyne_lowering_pass.h"
#include "logging.h"

namespace koda
{
std::string DezyneEmitter::id() const
{
  return "dezyne";
}

VoidResult DezyneEmitter::generate(const ir::Program& program, const SymbolRegistry& symbols, const CompilerOptions& options)
{
  mModel.clear();
  mGeneratedFiles.clear();

  dezyne::DeclarationPass declarations(mModel, symbols, options);
  RETURN_ON_FAILURE(declarations.run(program));

  dezyne::LoweringPass lowering(mModel, symbols, options);
  RETURN_ON_FAILURE(lowering.run(program));

  RETURN_ON_FAILURE(mWriter.write(mModel, options.dryRun));
  mGeneratedFiles = mWriter.generatedFiles();

  return {};
}

const std::vector<std::string>& DezyneEmitter::generatedFiles() const
{
  return mGeneratedFiles;
}

const dezyne::Model& DezyneEmitter::model() const
{
  return mModel;
}

}  // namespace koda
