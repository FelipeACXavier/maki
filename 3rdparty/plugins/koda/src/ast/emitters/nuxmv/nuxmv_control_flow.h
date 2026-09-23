#pragma once

#include "nuxmv_model.h"
#include "ast/koda_ir.h"

#include <functional>
#include <unordered_map>

namespace koda::nuxmv
{

// Context supplied by the enclosing compiler while lowering higher-level control flow.
// End and Continue are deliberately represented as semantic signals, not fake actions.
struct ControlFlowContext
{
  PExpression endSignal;
  PExpression continueSignal;
};

struct FlowBinding
{
  ir::PStrategy strategy;
};

using FlowTable = std::unordered_map<SymbolId, FlowBinding>;

PExpression chooseConditionPlaceholder(const std::string& ownerId, std::size_t option);

} // namespace koda::nuxmv
