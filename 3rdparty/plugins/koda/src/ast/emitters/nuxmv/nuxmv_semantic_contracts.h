#pragma once

#include <string>
#include <utility>

#include "nuxmv_model.h"

namespace koda::nuxmv::contracts
{
// These are verification SPECIFICATIONS, not TRANS constraints. Keep the
// reference-level obligations separate from next-state generation.
inline PExpression implies(PExpression condition, PExpression obligation)
{
  return binary(std::move(condition), "->", std::move(obligation));
}

inline void always(Behaviour& behaviour, const std::string& contract, PExpression obligation)
{
#ifdef GENERATE_CONTRACTS
  behaviour.expectations.push_back({
    .name = behaviour.id + "_sem_" + contract,
    .expression = unary("G", std::move(obligation)),
    .origin = behaviour.origin,
    .kind = PropertyKind::Ltl,
  });
#endif
}

inline void nextState(Behaviour& behaviour, const std::string& contract, PExpression condition, PExpression statePredicate)
{
  always(behaviour, contract, implies(std::move(condition), unary("X", std::move(statePredicate))));
}

// Conservative capability metadata describes POSSIBLE outcomes, not a proof
// that each outcome is reachable in every environment. Coverage is separate.
inline bool anyMayAccept(const std::vector<Behaviour>& children)
{
  for (const auto& c : children)
    if (c.mayAccept)
      return true;
  return false;
}

}  // namespace koda::nuxmv::contracts
