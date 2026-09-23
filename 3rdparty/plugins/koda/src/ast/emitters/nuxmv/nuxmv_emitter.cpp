#include "nuxmv_emitter.h"

#include "ast/koda_compiler.h"
#include "logging.h"
#include "nuxmv_semantic_compiler.h"

namespace koda
{
std::string NuXmvEmitter::id() const
{
  return "nuxmv";
}

VoidResult NuXmvEmitter::generate(const ir::Program& program, const SymbolRegistry& symbols, const koda::types::TypeRegistry&, const CompilerOptions& options)
{
  mModels.clear();
  mGeneratedFiles.clear();
  mWriter.clear();

  nuxmv::SemanticCompiler compiler(program, symbols, options);

  for (const auto& component : program.components)
  {
    if (component.kind != ir::ComponentKind::Task)
      continue;

    for (const auto& flow : component.flows)
    {
      if (flow.name != "main")
        continue;

      nuxmv::Model model;
      LOG_DEBUG("Compiling flow: {} at {}", flow.name, flow.span.toString());
      RETURN_ON_FAILURE(compiler.compileFlow(component, flow, model));
      RETURN_ON_FAILURE(compiler.compileProperties(component, model));
      RETURN_ON_FAILURE(mWriter.write(model, options.dryRun));

      mModels.push_back(std::move(model));
    }
  }

  mGeneratedFiles = mWriter.generatedFiles();
  return {};
}

const std::vector<std::string>& NuXmvEmitter::generatedFiles() const
{
  return mGeneratedFiles;
}

const std::vector<nuxmv::Model>& NuXmvEmitter::models() const
{
  return mModels;
}

}  // namespace koda
