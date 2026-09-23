#include <gtest/gtest.h>

#include <sstream>

#include "ast/emitters/nuxmv/nuxmv_model.h"
#include "ast/emitters/nuxmv/nuxmv_writer.h"

namespace
{
using namespace koda::nuxmv;

TEST(NuXmvWriter, WritesFlatMainModule)
{
  Model model;
  model.setName("leaf");

  model.addVariable({
      .kind = VariableKind::Input,
      .name = "command",
      .domain = "0..3",
  });
  model.addVariable({
      .kind = VariableKind::State,
      .name = "state",
      .domain = "{Idle, Error}",
      .initial = reference("Idle"),
  });
  model.addDefinition({"reply", reference("RESULT_NONE")});
  model.addNextAssignment({
      .variable = "state",
      .cases = {
          {equal(reference("command"), reference("CMD_TRIGGER")), reference("Error")},
      },
      .fallback = reference("state"),
  });

  std::ostringstream stream;
  Writer writer;
  ASSERT_TRUE(writer.write(model, stream).IsSuccess());

  const auto text = stream.str();
  EXPECT_NE(text.find("MODULE main"), std::string::npos);
  EXPECT_EQ(text.find("MODULE leaf"), std::string::npos);
  EXPECT_NE(text.find("IVAR"), std::string::npos);
  EXPECT_NE(text.find("VAR"), std::string::npos);
  EXPECT_NE(text.find("next(state)"), std::string::npos);
}

}  // namespace
