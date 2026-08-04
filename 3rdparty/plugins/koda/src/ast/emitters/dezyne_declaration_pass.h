#pragma once

#include <map>
#include <string>

#include "dezyne_model.h"
#include "koda_ir.h"
#include "result.h"
#include "symbol_registry.h"

namespace koda
{
struct CompilerOptions;
}

namespace koda::dezyne
{
class DeclarationPass
{
public:
  DeclarationPass(Model& model, const SymbolRegistry& symbols, const koda::CompilerOptions& options);
  VoidResult run(const ir::Program& program);

private:
  struct FlowState
  {
    SymbolId component = InvalidSymbol;
    std::map<koda::SymbolId, std::uint32_t> triggerOrdinals;
  };

  VoidResult declareCapability(const ir::Component& capability);
  VoidResult declareTask(const ir::Component& task);
  VoidResult declareFlow(const ir::Component& task, const ir::Flow& flow);
  VoidResult declareStrategy(const ir::PStrategy& strategy, FlowState& state);
  VoidResult declareHandler(const ir::PHandler& handler, FlowState& state);
  VoidResult declareCall(const ir::Call& call, FlowState& state, bool signal);

  std::string sourceName(koda::SymbolId id) const;
  static std::string lower(std::string value);
  static std::string componentName(const std::string& name);
  static std::string flowName(const std::string& name);

  Model& mModel;
  const SymbolRegistry& mSymbols;
  const koda::CompilerOptions& mOptions;
};

}  // namespace koda::dezyne
