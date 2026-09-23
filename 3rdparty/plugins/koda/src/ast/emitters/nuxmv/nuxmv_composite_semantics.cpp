#include "nuxmv_composite_semantics.h"

#include "nuxmv_constants.h"

namespace koda::nuxmv
{

PExpression synchronousReplyChain(const std::vector<Behaviour>& children,
                                  std::size_t first)
{
  if (first >= children.size())
    return constants::resultDone();

  auto reply = children.back().triggerReply;
  for (std::size_t n = children.size() - 1; n-- > first;)
  {
    reply = cases(
        {{equal(children[n].triggerReply, constants::resultDone()), reply}},
        children[n].triggerReply);
  }
  return reply;
}

PExpression allChildrenCompleted(const std::vector<PExpression>& completed)
{
  if (completed.empty())
    return boolean(true);

  auto result = completed.front();
  for (std::size_t i = 1; i < completed.size(); ++i)
    result = logicalAnd(result, completed[i]);
  return result;
}

PExpression anyChildFailed(const std::vector<Behaviour>& children)
{
  if (children.empty())
    return boolean(false);

  auto result = equal(children.front().outcome, constants::eventFailure());
  for (std::size_t i = 1; i < children.size(); ++i)
    result = logicalOr(result,
                       equal(children[i].outcome, constants::eventFailure()));
  return result;
}

PExpression exactlyOneEvent(const std::vector<Behaviour>& children)
{
  // Pairwise exclusion is sufficient here because EVENT_NONE is allowed.
  PExpression result = boolean(true);

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    for (std::size_t j = i + 1; j < children.size(); ++j)
    {
      const auto both =
          logicalAnd(
              logicalNot(equal(children[i].outcome, constants::eventNone())),
              logicalNot(equal(children[j].outcome, constants::eventNone())));
      result = logicalAnd(result, logicalNot(both));
    }
  }

  return result;
}

std::string childStateName(std::size_t index)
{
  return "Child" + std::to_string(index);
}

}  // namespace koda::nuxmv
