#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "emitters/emitter.h"
#include "koda_ir.h"
#include "koda_plugin.h"
#include "result.h"
#include "semantic_analyzer.h"
#include "symbol_registry.h"
#include "typing/blackboard_model.h"
#include "typing/type_registry.h"

namespace koda
{
struct CompilerOptions
{
  std::string inputFile;
  std::string outputDir = "./out";

  int verbose = 0;
  bool showHelp = false;
  bool dryRun = false;
  bool showVersion = false;

  enum class PluginOption
  {
    NoPlugins = 0,
    PluginsOnly,
    RunAll
  } pluginRule = PluginOption::NoPlugins;
};

class Compiler
{
public:
  Compiler();

  VoidResult parse(const CompilerOptions& options);
  VoidResult generate();

  const System& getAST() const
  {
    return mAST;
  }

  const SymbolRegistry& getSymbols() const
  {
    return mSymbols;
  }

  const SemanticModel& getSemantics() const
  {
    return mSemantics;
  }

  const ir::Program& getIR() const
  {
    return mIR;
  }

  void printAST() const;

  VoidResult addEmitter(std::shared_ptr<Emitter> emitter);
  VoidResult addPlugin(std::shared_ptr<KodaPlugin> plugin);
  std::vector<std::string> generatedFiles() const;

private:
  VoidResult runFrontend();
  VoidResult runEmitters();
  VoidResult runPlugins();

  static std::string toFilename(const std::string& name);
  static std::string componentName(const std::string& name);
  static std::string flowName(const std::string& name);

  CompilerOptions mOptions;
  System mAST;
  SymbolRegistry mSymbols;
  SemanticModel mSemantics;
  ir::Program mIR;

  std::shared_ptr<types::TypeRegistry> mTypeRegistry;
  std::shared_ptr<types::Blackboard> mBlackboard;
  std::vector<std::shared_ptr<Emitter>> mEmitters;
  std::map<std::string, std::shared_ptr<KodaPlugin>> mPlugins;
  std::vector<std::string> mGeneratedFiles;
};

}  // namespace koda
