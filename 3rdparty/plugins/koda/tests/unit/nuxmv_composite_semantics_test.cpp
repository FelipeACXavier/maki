#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_composite_semantics.h"
#include "ast/emitters/nuxmv/nuxmv_constants.h"

namespace
{
using namespace koda::nuxmv;

Behaviour child(const std::string& id)
{
  Behaviour result;
  result.id = id;
  result.triggerReply = reference(id + "_reply");
  result.outcome = reference(id + "_event");
  return result;
}

TEST(NuXmvCompositeSemantics, SynchronousReplyChainContinuesOnlyOnDone)
{
  const std::vector<Behaviour> children{child("a"), child("b"), child("c")};

  const auto text = toString(synchronousReplyChain(children, 0));

  EXPECT_NE(text.find("a_reply"), std::string::npos);
  EXPECT_NE(text.find("b_reply"), std::string::npos);
  EXPECT_NE(text.find("c_reply"), std::string::npos);
  EXPECT_NE(text.find("RESULT_DONE"), std::string::npos);
}

TEST(NuXmvCompositeSemantics, EmptyCompletionSetIsComplete)
{
  EXPECT_EQ(toString(allChildrenCompleted({})), "TRUE");
}

TEST(NuXmvCompositeSemantics, MultipleChildEventsAreExcluded)
{
  const std::vector<Behaviour> children{child("a"), child("b")};

  const auto text = toString(exactlyOneEvent(children));

  EXPECT_NE(text.find("a_event"), std::string::npos);
  EXPECT_NE(text.find("b_event"), std::string::npos);
  EXPECT_NE(text.find("EVENT_NONE"), std::string::npos);
}

TEST(NuXmvCompositeSemantics, FailureSummaryContainsEveryChild)
{
  const std::vector<Behaviour> children{child("a"), child("b"), child("c")};

  const auto text = toString(anyChildFailed(children));

  EXPECT_NE(text.find("a_event"), std::string::npos);
  EXPECT_NE(text.find("b_event"), std::string::npos);
  EXPECT_NE(text.find("c_event"), std::string::npos);
  EXPECT_NE(text.find("EVENT_FAILURE"), std::string::npos);
}

}  // namespace
