#include "tracer_emitter.h"

#include <filesystem>
#include <format>
#include <set>

#include "ast/koda_compiler.h"
#include "logging.h"
#include "tracer_serializer.h"

namespace koda
{
namespace
{
// Do not allow source-controlled task names to escape options.outputDir.
std::string safeFilename(const std::string& name)
{
  std::string out;
  for (unsigned char c : name)
  {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-')
      out.push_back(static_cast<char>(c));
    else
      out.push_back('_');
  }
  return out;
}
}  // namespace

std::string TracerEmitter::id() const
{
  return "tracer";
}

VoidResult TracerEmitter::generate(const ir::Program& program, const SymbolRegistry& symbols, const koda::types::TypeRegistry&, const CompilerOptions& options)
{
  mGeneratedFiles.clear();
  mWriter.clear();

  tracer::Serializer serializer(program, symbols);
  std::set<std::string> filenames;

  // Validate every task before touching the filesystem: no partial semantic projection
  // on UnsupportedNode, and dryRun performs exactly the same validation.
  struct Document
  {
    std::filesystem::path path;
    tracer::Json json;
  };
  std::vector<Document> documents;

  for (const auto& task : program.components)
  {
    if (task.kind != ir::ComponentKind::Task)
      continue;

    LOG_DEBUG("Serializing KODA task '{}' for semantic reference", task.name);
    auto result = serializer.serializeTask(task);
    if (!result.IsSuccess())
      return VoidResult::Failed(result.ErrorMessage());

    auto name = safeFilename(task.name);
    if (name.empty())
      return VoidResult::Failed("tracer: empty task filename");
    auto mocks = serializer.serializeDefaultMocks(task);
    if (!mocks.IsSuccess())
      return VoidResult::Failed(mocks.ErrorMessage());

    const auto referenceFilename = name + ".reference.json";
    const auto mockFilename = name + ".default-mocks.json";
    if (!filenames.insert(referenceFilename).second || !filenames.insert(mockFilename).second)
      return VoidResult::Failed(std::format("tracer: output filename collision for task '{}'", task.name));

    documents.push_back({std::filesystem::path(options.outputDir) / referenceFilename, std::move(result.Value())});
    documents.push_back({std::filesystem::path(options.outputDir) / mockFilename, std::move(mocks.Value())});
  }

  for (const auto& doc : documents)
  {
    RETURN_ON_FAILURE(mWriter.write(doc.path, doc.json, options.dryRun));
  }

  mGeneratedFiles = mWriter.generatedFiles();
  return {};
}

const std::vector<std::string>& TracerEmitter::generatedFiles() const
{
  return mGeneratedFiles;
}
}  // namespace koda
