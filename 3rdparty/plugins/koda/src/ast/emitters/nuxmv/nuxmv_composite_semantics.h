#pragma once

#include "nuxmv_model.h"

#include <cstddef>
#include <string>
#include <vector>

namespace koda::nuxmv
{

// Helpers shared by Repeat, Join and Either lowering.
//
// These functions construct expressions only. They do not introduce nested SMV modules.
// The SemanticCompiler remains responsible for compiling AST children into Behaviour
// fragments and merging those fragments into the final flat Model.

PExpression synchronousReplyChain(const std::vector<Behaviour>& children,
                                  std::size_t first);

PExpression allChildrenCompleted(const std::vector<PExpression>& completed);

PExpression anyChildFailed(const std::vector<Behaviour>& children);

PExpression exactlyOneEvent(const std::vector<Behaviour>& children);

std::string childStateName(std::size_t index);

}  // namespace koda::nuxmv
