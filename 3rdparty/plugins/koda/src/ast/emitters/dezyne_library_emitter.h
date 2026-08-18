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

VoidResult createComponent(Model& model, const std::string& outdir, const std::string& name,
                           std::function<void(const std::string& name, const std::string& path, std::ostringstream& out)> callback);
VoidResult createTypes(Model& model, const std::string& outdir);
VoidResult createActionInterface(Model& model, const std::string& outdir);
VoidResult createSignalInterface(Model& model, const std::string& outdir);

// Main orchestration components
VoidResult createSequenceComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
VoidResult createParallelComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
VoidResult createEveryComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
VoidResult createWithinComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
VoidResult createRepeatComponent(Model& model, const std::string& outdir, SymbolId componentId);
VoidResult createEveryComponent(Model& model, const std::string& outdir, SymbolId componentId);
VoidResult createSignalHandlerComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
VoidResult createSignalContinueComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);
VoidResult createAbortHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId);
VoidResult createErrorHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId);

// Helpers
VoidResult createAlarmComponent(Model& model, const std::string& outdir);
VoidResult createAlarmInterface(Model& model, const std::string& outdir);
VoidResult createActionArbiterComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId);

void createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent);
void createParallelDoneRecursion(bool fromIdle, bool fromDone, uint32_t start, uint32_t instances, std::ostringstream& out,
                                 const std::string& indent);

}  // namespace koda::dezyne