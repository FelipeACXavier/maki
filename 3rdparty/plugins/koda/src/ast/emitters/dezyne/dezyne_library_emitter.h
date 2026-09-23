#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include "dezyne_model.h"
#include "result.h"

namespace koda::dezyne
{

struct LibraryPort
{
  std::string name;
  PortProtocol kind = PortProtocol::Action;
};

struct LibraryComponent
{
  std::string name = "";
  std::string filename = "";
  std::string path = "";
  std::vector<LibraryPort> providesPorts = {};
  std::vector<LibraryPort> requiresPorts = {};
  SymbolId symbol = InvalidSymbol;
};

Result<LibraryComponent> createComponent(Model& model, const std::string& outdir, const std::string& name, SymbolId componentId,
                                         std::function<void(LibraryComponent& name, std::ostringstream& out)> callback);
VoidResult createTypes(Model& model, const std::string& outdir);
VoidResult createAbortInterface(Model& model, const std::string& outdir);
VoidResult createActionInterface(Model& model, const std::string& outdir);
VoidResult createSignalInterface(Model& model, const std::string& outdir);
VoidResult createExternalInterface(Model& model, const std::string& outdir);
VoidResult createConditionInterface(Model& model, const std::string& outdir);

// Main orchestration components
Result<LibraryComponent> createSequenceComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
Result<LibraryComponent> createParallelComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
Result<LibraryComponent> createWithinComponent(Model& model, const std::string& outdir, int timeout, SymbolId componentId);
Result<LibraryComponent> createRepeatComponent(Model& model, const std::string& outdir, SymbolId componentId);
Result<LibraryComponent> createEveryComponent(Model& model, const std::string& outdir, SymbolId componentId);
Result<LibraryComponent> createSignalHandlerComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
Result<LibraryComponent> createSignalContinueComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
Result<LibraryComponent> createAbortHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId);
Result<LibraryComponent> createErrorHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId);

Result<LibraryComponent> createFailureComponent(Model& model, const std::string& outdir, SymbolId componentId);
Result<LibraryComponent> createSelectorComponent(Model& model, const std::string& outdir, uint32_t conditionCount, uint32_t branchCount, SymbolId componentId);
Result<LibraryComponent> createAbortCallComponent(Model& model, const std::string& outdir, SymbolId componentId);
Result<LibraryComponent> createFlowArmour(Model& model, const std::string& outdir, SymbolId componentId);
Result<LibraryComponent> createCapabilityArmour(Model& model, const std::string& outdir, const std::string& capabilityName, bool hasAbort,
                                                const std::vector<std::string>& ports, SymbolId componentId);

// Helpers
VoidResult createAlarmComponent(Model& model, const std::string& outdir);
VoidResult createAlarmInterface(Model& model, const std::string& outdir);
Result<LibraryComponent> createActionArbiterComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
Result<LibraryComponent> createAbortArbiterComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
Result<LibraryComponent> createConditionComponent(Model& model, const std::string& outdir, const std::string& name, SymbolId componentId);

void createSelectorRecursion(uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent);
void createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent);
void createParallelDoneRecursion(bool fromIdle, bool fromDone, uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent);

}  // namespace koda::dezyne