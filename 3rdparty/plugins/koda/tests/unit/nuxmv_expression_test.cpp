#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_expression.h"

namespace
{
using namespace koda::nuxmv;

TEST(NuXmvExpression, RendersBinaryExpression)
{
  const auto expression = logicalAnd(
      equal(reference("state"), reference("Idle")),
      equal(reference("command"), reference("CMD_TRIGGER")));

  EXPECT_EQ(toString(expression), "((state = Idle) & (command = CMD_TRIGGER))");
}

TEST(NuXmvExpression, RendersCaseExpression)
{
  const auto expression = cases(
      {
          {equal(reference("state"), reference("Idle")), reference("RESULT_DONE")},
          {equal(reference("state"), reference("Error")), reference("RESULT_FAILURE")},
      },
      reference("RESULT_NONE"));

  const auto text = toString(expression);
  EXPECT_NE(text.find("state = Idle"), std::string::npos);
  EXPECT_NE(text.find("RESULT_DONE"), std::string::npos);
  EXPECT_NE(text.find("TRUE : RESULT_NONE"), std::string::npos);
}
}  // namespace
