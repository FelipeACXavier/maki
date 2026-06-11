#include <gtest/gtest.h>

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QSignalSpy>
#include <QString>

#include "helpers/widget_helpers.h"
#include "logging.h"
#include "widgets/dropdown_button.h"

const auto OPTION_1 = "Pipeline A";
const auto OPTION_2 = "Pipeline A";

void triggerSubmenuAction(DropDownButton& button, const QString& optionName, const QString& actionName)
{
  auto* submenu = maki::testing::submenuForOption(button.menu(), optionName);
  if (submenu == nullptr)
    FAIL() << "DropDownButton has no submenu:" << optionName.toStdString();

  auto* action = maki::testing::actionWithText(submenu, actionName);
  if (submenu == nullptr)
    FAIL() << "DropDownButton has no action:" << actionName.toStdString();

  action->trigger();
}

TEST(DropDownButtonTests, DefaultStateUsesAddNewAsCurrentOption)
{
  DropDownButton button;

  EXPECT_EQ(button.currentOption(), QString("Add new..."));
  EXPECT_EQ(button.text(), QString("Add new..."));
}

TEST(DropDownButtonTests, AddOptionSelectsFirstOption)
{
  DropDownButton button;

  button.addOption(OPTION_1);

  EXPECT_EQ(button.currentOption(), OPTION_1);
  EXPECT_EQ(button.text(), OPTION_1);
}

TEST(DropDownButtonTests, SetCurrentOptionChangesCurrentOption)
{
  DropDownButton button;

  LOG_INFO("Add the first option");
  button.addOption(OPTION_1);

  LOG_INFO("Add the second option and make sure the first option is still set");
  button.addOption(OPTION_2);

  LOG_INFO("Verify that the first option is still set");
  EXPECT_EQ(button.currentOption(), OPTION_1);
  EXPECT_EQ(button.text(), OPTION_1);

  LOG_INFO("Set the option as the second");
  button.setCurrentOption(OPTION_2);

  LOG_INFO("Verify that the second option is now set");
  EXPECT_EQ(button.currentOption(), OPTION_2);
  EXPECT_EQ(button.text(), OPTION_2);
}

TEST(DropDownButtonTests, ClickingButtonEmitsExecuteRequestedForCurrentOption)
{
  DropDownButton button;

  LOG_INFO("Add the first option");
  button.addOption(OPTION_1);

  LOG_INFO("Connect a spy signal");
  QSignalSpy spy(&button, &DropDownButton::executeRequested);
  ASSERT_TRUE(spy.isValid());

  LOG_INFO("Click the button");
  button.click();

  LOG_INFO("Verify that the execute signal was emitted and that it has the right option");
  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_1);
}

TEST(DropDownButtonTests, RunActionEmitsExecuteRequestedForSelectedOption)
{
  DropDownButton button;
  LOG_INFO("Add the first and second options");
  button.addOption(OPTION_1);
  button.addOption(OPTION_2);

  LOG_INFO("Connect a spy signal");
  QSignalSpy spy(&button, &DropDownButton::executeRequested);
  ASSERT_TRUE(spy.isValid());

  LOG_INFO("Run the second option from the button dropdown menu");
  triggerSubmenuAction(button, OPTION_2, "Run");

  LOG_INFO("Verify that the signal was emitted with the second option");
  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_2);
  EXPECT_EQ(button.currentOption(), OPTION_2);
}

TEST(DropDownButtonTests, SelectActionChangesCurrentOptionWithoutExecuting)
{
  DropDownButton button;
  LOG_INFO("Add the first and second options");
  button.addOption(OPTION_1);
  button.addOption(OPTION_2);

  QSignalSpy executeSpy(&button, &DropDownButton::executeRequested);
  ASSERT_TRUE(executeSpy.isValid());

  triggerSubmenuAction(button, OPTION_2, "Select");

  EXPECT_EQ(executeSpy.count(), 0);
  EXPECT_EQ(button.currentOption(), OPTION_2);
}

TEST(DropDownButtonTests, EditActionEmitsEditOptionRequested)
{
  DropDownButton button;
  button.addOption(OPTION_1);

  QSignalSpy spy(&button, &DropDownButton::editOptionRequested);
  ASSERT_TRUE(spy.isValid());

  triggerSubmenuAction(button, OPTION_1, "Edit");

  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_1);
}

TEST(DropDownButtonTests, DeleteActionEmitsDeleteOptionRequested)
{
  DropDownButton button;
  button.addOption(OPTION_1);

  QSignalSpy spy(&button, &DropDownButton::deleteOptionRequested);
  ASSERT_TRUE(spy.isValid());

  triggerSubmenuAction(button, OPTION_1, "Delete");

  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_1);

  button.removeOption(OPTION_1);
  EXPECT_EQ(button.currentOption(), QString("Add new..."));
  EXPECT_EQ(button.text(), QString("Add new..."));
}
