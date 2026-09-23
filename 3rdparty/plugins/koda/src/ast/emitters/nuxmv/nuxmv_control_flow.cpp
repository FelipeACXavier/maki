#include "nuxmv_control_flow.h"

namespace koda::nuxmv
{

PExpression chooseConditionPlaceholder(const std::string& ownerId, std::size_t option)
{
  // Until data-expression lowering is connected, conditions are explicit IVAR-like
  // symbolic inputs rather than silently guessed Boolean translations.
  return reference(ownerId + "_condition" + std::to_string(option));
}

} // namespace koda::nuxmv
