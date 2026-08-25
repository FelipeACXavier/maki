#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include "result.h"

namespace koda::dezyne
{

// Main orchestration components
VoidResult createSequenceComponent(uint32_t instances, std::ostringstream& out);
VoidResult createParallelComponent(uint32_t instances, std::ostringstream& out);
VoidResult createEveryComponent(uint32_t instances, std::ostringstream& out);
VoidResult createWithinComponent(uint32_t instances, std::ostringstream& out);
VoidResult createRepeatComponent(uint32_t instances, std::ostringstream& out);
VoidResult createSignalHandlerComponent(uint32_t instances, std::ostringstream& out);
VoidResult createSignalContinueComponent(uint32_t instances, std::ostringstream& out);
VoidResult createAbortHandlerComponent(uint32_t instances, std::ostringstream& out);
VoidResult createErrorHandlerComponent(uint32_t instances, std::ostringstream& out);

// Helpers
VoidResult createAlarmComponent(std::ostringstream& out);
VoidResult createAlarmInterface(std::ostringstream& out);
VoidResult createActionArbiterComponent(uint32_t instances, std::ostringstream& out);

void createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent);

}  // namespace koda::dezyne