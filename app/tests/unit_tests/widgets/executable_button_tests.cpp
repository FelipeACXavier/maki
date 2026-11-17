#include <gtest/gtest.h>

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QSignalSpy>
#include <QString>

#include "app_configs.h"
#include "helpers/qt_gtest_printers.h"
#include "helpers/widget_helpers.h"
#include "logging.h"
#include "widgets/execute_button.h"

const QString OPTION_1 = "Pipeline A";
const QString OPTION_1_ID = "Pipeline A - ID";
const QString OPTION_2 = "Pipeline B";
const QString OPTION_2_ID = "Pipeline B - ID";

void triggerSubmenuAction(ExecuteButton& button, const QString& optionName, const QString& actionName)
{
  auto* submenu = maki::testing::submenuForOption(button.menu(), optionName);
  if (submenu == nullptr)
    FAIL() << "ExecuteButton has no submenu:" << optionName.toStdString();

  auto* action = maki::testing::actionWithText(submenu, actionName);
  if (submenu == nullptr)
    FAIL() << "ExecuteButton has no action:" << actionName.toStdString();

  action->trigger();
}

TEST(ExecuteButtonTests, DefaultStateUsesAddNewAsCurrentOption)
{
  ExecuteButton button;

  EXPECT_EQ(button.currentOption().name, Constants::EXECUTABLE_BUTTON_DEFAULT_TEXT);
  EXPECT_EQ(button.text(), Constants::EXECUTABLE_BUTTON_DEFAULT_TEXT);
}

TEST(ExecuteButtonTests, AddOptionSelectsFirstOption)
{
  ExecuteButton button;

  button.addOption(OPTION_1, OPTION_1_ID);

  EXPECT_EQ(button.currentOption().name, OPTION_1);
  EXPECT_EQ(button.text(), OPTION_1);
}

TEST(ExecuteButtonTests, ClickingButtonEmitsExecuteRequestedForCurrentOption)
{
  ExecuteButton button;

  LOG_INFO("Add the first option");
  button.addOption(OPTION_1, OPTION_1_ID);

  LOG_INFO("Connect a spy signal");
  QSignalSpy spy(&button, &ExecuteButton::executeRequested);
  ASSERT_TRUE(spy.isValid());

  LOG_INFO("Click the button");
  button.click();

  LOG_INFO("Verify that the execute signal was emitted and that it has the right option");
  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_1_ID);
}

TEST(ExecuteButtonTests, RunActionEmitsExecuteRequestedForSelectedOption)
{
  ExecuteButton button;
  LOG_INFO("Add the first and second options");
  button.addOption(OPTION_1, OPTION_1_ID);
  button.addOption(OPTION_2, OPTION_2_ID);

  LOG_INFO("Connect a spy signal");
  QSignalSpy spy(&button, &ExecuteButton::executeRequested);
  ASSERT_TRUE(spy.isValid());

  LOG_INFO("Run the second option from the button dropdown menu");
  triggerSubmenuAction(button, OPTION_2, "Run");

  LOG_INFO("Verify that the signal was emitted with the second option");
  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_2_ID);
  EXPECT_EQ(button.currentOption().name, OPTION_2);
}

TEST(ExecuteButtonTests, SelectActionChangesCurrentOptionWithoutExecuting)
{
  ExecuteButton button;
  LOG_INFO("Add the first and second options");
  button.addOption(OPTION_1, OPTION_1_ID);
  button.addOption(OPTION_2, OPTION_2_ID);

  QSignalSpy executeSpy(&button, &ExecuteButton::executeRequested);
  ASSERT_TRUE(executeSpy.isValid());

  triggerSubmenuAction(button, OPTION_2, "Select");

  EXPECT_EQ(executeSpy.count(), 0);
  EXPECT_EQ(button.currentOption().name, OPTION_2);
}

TEST(ExecuteButtonTests, EditActionEmitsEditOptionRequested)
{
  ExecuteButton button;
  button.addOption(OPTION_1, OPTION_1_ID);

  QSignalSpy spy(&button, &ExecuteButton::editOptionRequested);
  ASSERT_TRUE(spy.isValid());

  triggerSubmenuAction(button, OPTION_1, "Edit");

  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_1_ID);
}

TEST(ExecuteButtonTests, DeleteActionEmitsDeleteOptionRequested)
{
  ExecuteButton button;
  LOG_INFO("Add the first option");
  button.addOption(OPTION_1, OPTION_1_ID);

  LOG_INFO("Setup spy for deleteOptionRequested");
  QSignalSpy spy(&button, &ExecuteButton::deleteOptionRequested);
  ASSERT_TRUE(spy.isValid());

  LOG_INFO("Trigger the delete action");
  triggerSubmenuAction(button, OPTION_1, "Delete");

  LOG_INFO("Verify that the action was triggered");
  ASSERT_EQ(spy.count(), 1);
  EXPECT_EQ(spy.takeFirst().at(0).toString(), OPTION_1_ID);

  LOG_INFO("Remove the options");
  button.removeOption(OPTION_1_ID);
  EXPECT_EQ(button.currentOption().name, Constants::EXECUTABLE_BUTTON_DEFAULT_TEXT);
  EXPECT_EQ(button.text(), Constants::EXECUTABLE_BUTTON_DEFAULT_TEXT);
}