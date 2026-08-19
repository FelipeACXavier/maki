#include "koda_compiler.h"

#include <fstream>

#include "antlr4-runtime.h"
#include "cst2ast.h"
#include "declaration_pass.h"
#include "emitters/dezyne_emitter.h"
#include "error_listener.h"
#include "ir_builder.h"
#include "logging.h"
#include "parser/KodaLexer.h"
#include "parser/KodaParser.h"

namespace koda
{
Compiler::Compiler()
{
  mEmitters.push_back(std::make_shared<DezyneEmitter>());
}

VoidResult Compiler::parse(const CompilerOptions& options)
{
  mOptions = options;
  LOG_DEBUG("Parsing {}", options.inputFile);

  std::ifstream stream(options.inputFile);
  if (!stream.is_open())
    return VoidResult::Failed("Failed to open: " + options.inputFile);

  antlr4::ANTLRInputStream input(stream);
  KodaLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  KodaParser parser(&tokens);

  CollectingErrorListener errorListener;
  parser.removeErrorListeners();
  parser.addErrorListener(&errorListener);

  KodaParser::SystemContext* tree = nullptr;
  try
  {
    tree = parser.system();
  } catch (const std::exception& e)
  {
    return VoidResult::Failed("Failed to parse input file: {}", e.what());
  }

  if (errorListener.hasErrors() || tree == nullptr)
  {
    for (const auto& err : errorListener.errors)
      LOG_WARNING(err);

    return VoidResult::Failed("Failed to parse input file");
  }

  mBlackboard = std::make_shared<koda::types::Blackboard>();
  mTypeRegistry = std::make_shared<koda::types::TypeRegistry>();
  koda::CST2AST visitor(mTypeRegistry, &errorListener);
  try
  {
    mAST = std::any_cast<System>(visitor.build(tree));
  } catch (const std::bad_any_cast& e)
  {
    return VoidResult::Failed(std::string("Failed to build AST: ") + e.what());
  } catch (const std::invalid_argument& e)
  {
    return VoidResult::Failed(std::string("Failed to build AST: ") + e.what());
  } catch (const std::runtime_error& e)
  {
    return VoidResult::Failed(std::string("Failed to build AST: ") + e.what());
  }

  if (errorListener.hasErrors())
  {
    for (const auto& err : errorListener.errors)
      LOG_WARNING(err);

    return VoidResult::Failed("Failed to build AST");
  }

  if (mOptions.verbose > 0)
    printAST();

  return VoidResult{};
}

VoidResult Compiler::generate()
{
  mGeneratedFiles.clear();

  auto frontend = runFrontend();
  if (!frontend.IsSuccess())
    return frontend;

  if (mOptions.pluginRule == CompilerOptions::PluginOption::PluginsOnly)
    return runPlugins();

  auto emitted = runEmitters();
  if (!emitted.IsSuccess())
    return emitted;

  if (mOptions.pluginRule == CompilerOptions::PluginOption::RunAll)
    return runPlugins();

  return VoidResult{};
}

VoidResult Compiler::runFrontend()
{
  // Populate the symbol registry with the necessary information for lookup later, e.g., capabilities, flows,
  // tasks, args... Basically, anything that can be references later
  DeclarationPass declarations(mSymbols, *mTypeRegistry);
  auto declared = declarations.run(mAST);
  if (!declared.IsSuccess())
    return declared;

  // mSymbols.print();

  // With the registry created, we can verify if the semantics of the KODA program make sense
  SemanticAnalyzer semantics(mSymbols, *mTypeRegistry, *mBlackboard);
  auto analyzed = semantics.run(mAST);
  if (!analyzed.IsSuccess())
    return analyzed;

  mSemantics = semantics.model();

  // Finally, we build the intermediate representation that emitters can use
  IRBuilder builder(mSymbols, mSemantics);
  auto ir = builder.build(mAST);
  if (!ir.IsSuccess())
    return VoidResult::Failed(ir.ErrorMessage());

  mIR = ir.Value();

  return VoidResult{};
}

VoidResult Compiler::runEmitters()
{
  for (const auto& emitter : mEmitters)
  {
    auto result = emitter->generate(mIR, mSymbols, mOptions);
    if (!result.IsSuccess())
      return result;

    const auto& files = emitter->generatedFiles();
    mGeneratedFiles.insert(mGeneratedFiles.end(), files.begin(), files.end());
  }
  return VoidResult{};
}

VoidResult Compiler::addEmitter(std::shared_ptr<Emitter> emitter)
{
  if (!emitter)
    return VoidResult::Failed("Invalid, null emitter");

  for (const auto& existing : mEmitters)
    if (existing->id() == emitter->id())
      return VoidResult::Failed("Emitter already exists: " + emitter->id());

  mEmitters.push_back(std::move(emitter));
  return VoidResult{};
}

VoidResult Compiler::addPlugin(std::shared_ptr<KodaPlugin> plugin)
{
  if (!plugin)
    return VoidResult::Failed("Invalid, null plugin");
  if (mPlugins.contains(plugin->id()))
    return VoidResult::Failed("Plugin already exists");

  plugin->toFilename = [](const std::string& name) { return toFilename(name); };
  plugin->componentName = [](const std::string& name) { return componentName(name); };
  plugin->flowName = [](const std::string& name) { return flowName(name); };
  mPlugins[plugin->id()] = std::move(plugin);
  return VoidResult{};
}

VoidResult Compiler::runPlugins()
{
  // Compatibility path: existing plugins still consume the AST. A future plugin-v2
  // interface can consume the semantic model or Koda IR without changing emitters.
  for (const auto& [_, plugin] : mPlugins)
  {
    auto result = plugin->generate(mOptions, mAST);
    if (!result.IsSuccess())
      return result;
  }
  return VoidResult{};
}

std::vector<std::string> Compiler::generatedFiles() const
{
  return mGeneratedFiles;
}

void Compiler::printAST() const
{
  mTypeRegistry->print();
  mAST.print();
}

std::string Compiler::toFilename(const std::string& name)
{
  if (name.empty())
    return name;
  std::string result = name;
  for (auto& c : result)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return result;
}

std::string Compiler::componentName(const std::string& name)
{
  return "c" + toFilename(name);
}

std::string Compiler::flowName(const std::string& name)
{
  return "f" + toFilename(name);
}

}  // namespace koda
