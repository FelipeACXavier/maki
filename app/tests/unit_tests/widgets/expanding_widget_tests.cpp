
#include <gtest/gtest.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QSignalSpy>
#include <QVBoxLayout>

#include "logging.h"
#include "widgets/clickable_icon.h"
#include "widgets/collapsible_area.h"
#include "widgets/expanding_widget.h"

using Direction = ExpandingWidget::Direction;

constexpr int ANIMATION_TIMEOUT_MS = 300;

// Wait for an expansion or collapse animation to finish.
bool waitForTransition(ExpandingWidget& widget, bool expand)
{
  auto* animation = widget.findChild<QPropertyAnimation*>();
  if (!animation)
    return false;

  QSignalSpy finished(animation, &QAbstractAnimation::finished);
  if (!finished.isValid())
    return false;

  if (expand)
    widget.expandArea();
  else
    widget.collapseArea();

  return finished.wait(ANIMATION_TIMEOUT_MS);
}

// Parameterized tests for all expansion directions.
class ExpandingWidgetTest : public ::testing::TestWithParam<Direction>
{
};

// ------------------------------------------------------------
// Construction
// ------------------------------------------------------------

TEST_P(ExpandingWidgetTest, StartsCollapsed)
{
  ExpandingWidget widget(GetParam());

  auto* animation = widget.findChild<QPropertyAnimation*>();
  ASSERT_NE(animation, nullptr);

  if (GetParam() == Direction::Up)
  {
    auto* area = widget.findChild<CollapsibleAreaHeight*>();
    ASSERT_NE(area, nullptr);

    EXPECT_EQ(area->contentHeight(), 0);
    EXPECT_NE(widget.getContent(), nullptr);
    EXPECT_EQ(widget.findChild<ClickableIcon*>(), nullptr);
  }
  else
  {
    auto* area = widget.findChild<CollapsibleAreaWidth*>();
    auto* button = widget.findChild<ClickableIcon*>();

    ASSERT_NE(area, nullptr);
    ASSERT_NE(button, nullptr);

    EXPECT_EQ(area->contentWidth(), 0);
    EXPECT_FALSE(button->isChecked());
    EXPECT_EQ(widget.getContent(), nullptr);
  }
}

INSTANTIATE_TEST_SUITE_P(AllDirections, ExpandingWidgetTest, ::testing::Values(Direction::Up, Direction::Left, Direction::Right),
                         [](const testing::TestParamInfo<Direction>& info) {
                           switch (info.param)
                           {
                             case Direction::Up:
                               return "Up";
                             case Direction::Left:
                               return "Left";
                             case Direction::Right:
                               return "Right";
                           }

                           return "Unknown";
                         });

// ------------------------------------------------------------
// Layout
// ------------------------------------------------------------

TEST(ExpandingWidgetTests, RightDirectionPlacesButtonBeforeContent)
{
  ExpandingWidget widget(Direction::Right);

  auto* layout = qobject_cast<QHBoxLayout*>(widget.layout());
  auto* button = widget.findChild<ClickableIcon*>();
  auto* area = widget.findChild<CollapsibleAreaWidth*>();

  ASSERT_NE(layout, nullptr);
  ASSERT_NE(button, nullptr);
  ASSERT_NE(area, nullptr);

  ASSERT_EQ(layout->count(), 2);

  EXPECT_EQ(layout->itemAt(0)->widget(), button);
  EXPECT_EQ(layout->itemAt(1)->widget(), area);
}

TEST(ExpandingWidgetTests, LeftDirectionPlacesButtonAfterContent)
{
  ExpandingWidget widget(Direction::Left);

  auto* layout = qobject_cast<QHBoxLayout*>(widget.layout());
  auto* button = widget.findChild<ClickableIcon*>();
  auto* area = widget.findChild<CollapsibleAreaWidth*>();

  ASSERT_NE(layout, nullptr);
  ASSERT_NE(button, nullptr);
  ASSERT_NE(area, nullptr);

  ASSERT_EQ(layout->count(), 2);

  EXPECT_EQ(layout->itemAt(0)->widget(), area);
  EXPECT_EQ(layout->itemAt(1)->widget(), button);
}

// ------------------------------------------------------------
// Content
// ------------------------------------------------------------

TEST(ExpandingWidgetTests, AddsWidgetsToVerticalContent)
{
  ExpandingWidget widget(Direction::Up);

  auto* first = new QLineEdit();
  auto* second = new QLineEdit();

  widget.addCollapsableWidget(first);
  widget.addCollapsableWidget(second);

  auto* content = widget.getContent();
  ASSERT_NE(content, nullptr);

  ASSERT_EQ(content->count(), 2);
  EXPECT_EQ(content->itemAt(0)->widget(), first);
  EXPECT_EQ(content->itemAt(1)->widget(), second);

  EXPECT_EQ(first->parentWidget(), second->parentWidget());
}

TEST(ExpandingWidgetTests, AddsOptionalLabelBeforeWidget)
{
  ExpandingWidget widget(Direction::Up);

  auto* field = new QLineEdit();
  widget.addCollapsableWidget(field, "Search");

  auto* content = widget.getContent();
  ASSERT_NE(content, nullptr);

  ASSERT_EQ(content->count(), 2);
  EXPECT_NE(content->itemAt(0)->widget(), nullptr);
  EXPECT_EQ(content->itemAt(1)->widget(), field);
}

TEST(ExpandingWidgetTests, AddsWidgetToHorizontalContent)
{
  ExpandingWidget widget(Direction::Right);

  auto* field = new QLineEdit();
  widget.addCollapsableWidget(field);

  auto* contentWidget = field->parentWidget();
  ASSERT_NE(contentWidget, nullptr);

  auto* layout = qobject_cast<QHBoxLayout*>(contentWidget->layout());
  ASSERT_NE(layout, nullptr);

  ASSERT_EQ(layout->count(), 1);
  EXPECT_EQ(layout->itemAt(0)->widget(), field);
}

// ------------------------------------------------------------
// Animation and dimensions
// ------------------------------------------------------------

TEST(ExpandingWidgetTests, ExpandsAndCollapsesHorizontally)
{
  ExpandingWidget widget(Direction::Right);
  widget.setExpandedSize(180);

  auto* area = widget.findChild<CollapsibleAreaWidth*>();
  ASSERT_NE(area, nullptr);

  ASSERT_TRUE(waitForTransition(widget, true));
  EXPECT_EQ(area->contentWidth(), 180);

  ASSERT_TRUE(waitForTransition(widget, false));
  EXPECT_EQ(area->contentWidth(), 0);
}

TEST(ExpandingWidgetTests, ExpandsAndCollapsesVertically)
{
  ExpandingWidget widget(Direction::Up);
  widget.setExpandedSize(120);

  auto* area = widget.findChild<CollapsibleAreaHeight*>();
  ASSERT_NE(area, nullptr);

  ASSERT_TRUE(waitForTransition(widget, true));
  EXPECT_EQ(area->contentHeight(), 120);

  ASSERT_TRUE(waitForTransition(widget, false));
  EXPECT_EQ(area->contentHeight(), 0);
}

TEST(ExpandingWidgetTests, ChangingExpandedSizeUpdatesExpandedArea)
{
  ExpandingWidget widget(Direction::Left);
  widget.setExpandedSize(150);

  auto* area = widget.findChild<CollapsibleAreaWidth*>();
  ASSERT_NE(area, nullptr);

  ASSERT_TRUE(waitForTransition(widget, true));
  ASSERT_EQ(area->contentWidth(), 150);

  widget.setExpandedSize(220);

  EXPECT_EQ(area->contentWidth(), 220);
  EXPECT_EQ(area->maximumWidth(), 220);
}

TEST(ExpandingWidgetTests, NegativeExpandedSizeIsClampedToZero)
{
  ExpandingWidget horizontal(Direction::Right);
  ExpandingWidget vertical(Direction::Up);

  horizontal.setExpandedSize(-100);
  vertical.setExpandedSize(-100);

  auto* widthArea = horizontal.findChild<CollapsibleAreaWidth*>();
  auto* heightArea = vertical.findChild<CollapsibleAreaHeight*>();

  ASSERT_NE(widthArea, nullptr);
  ASSERT_NE(heightArea, nullptr);

  EXPECT_EQ(widthArea->maximumWidth(), 0);
  EXPECT_EQ(heightArea->maximumHeight(), 0);
}

// ------------------------------------------------------------
// Button
// ------------------------------------------------------------

TEST(ExpandingWidgetTests, ProgrammaticExpansionUpdatesButton)
{
  ExpandingWidget widget(Direction::Right);

  auto* button = widget.findChild<ClickableIcon*>();
  ASSERT_NE(button, nullptr);

  EXPECT_FALSE(button->isChecked());

  ASSERT_TRUE(waitForTransition(widget, true));
  EXPECT_TRUE(button->isChecked());

  ASSERT_TRUE(waitForTransition(widget, false));
  EXPECT_FALSE(button->isChecked());
}

TEST(ExpandingWidgetTests, ButtonTooltipCanBeChanged)
{
  ExpandingWidget widget(Direction::Right);

  auto* button = widget.findChild<ClickableIcon*>();
  ASSERT_NE(button, nullptr);

  widget.setButtonTooltip("Expand search");

  EXPECT_EQ(button->toolTip(), "Expand search");
  EXPECT_EQ(button->toolTipDuration(), 2000);
}

TEST(ExpandingWidgetTests, ButtonToggleTriggersWidget)
{
  ExpandingWidget widget(Direction::Right);

  auto* button = widget.findChild<ClickableIcon*>();
  ASSERT_NE(button, nullptr);
  auto* animation = widget.findChild<QPropertyAnimation*>();
  ASSERT_NE(animation, nullptr);

  QSignalSpy expandedSpy(&widget, &ExpandingWidget::areaExpanded);
  QSignalSpy collapsedSpy(&widget, &ExpandingWidget::areaCollapsed);
  QSignalSpy toggleSpy(button, &ClickableIcon::toggled);
  QSignalSpy animationSpy(animation, &QAbstractAnimation::finished);

  ASSERT_TRUE(expandedSpy.isValid());
  ASSERT_TRUE(collapsedSpy.isValid());
  ASSERT_TRUE(toggleSpy.isValid());
  ASSERT_TRUE(animationSpy.isValid());

  LOG_INFO("Trigger the expansion by clicking the button");
  button->setChecked(true);

  EXPECT_TRUE(animationSpy.wait(ANIMATION_TIMEOUT_MS));

  LOG_INFO("Verify that the signal was emitted");
  ASSERT_EQ(toggleSpy.count(), 1);
  ASSERT_EQ(expandedSpy.count(), 1);
  EXPECT_EQ(collapsedSpy.count(), 0);
  EXPECT_TRUE(button->isChecked());

  LOG_INFO("Trigger the collapse by clicking the button");
  button->setChecked(false);

  EXPECT_TRUE(animationSpy.wait(ANIMATION_TIMEOUT_MS));

  LOG_INFO("Verify that the signal was emitted");
  ASSERT_EQ(toggleSpy.count(), 2);
  ASSERT_EQ(expandedSpy.count(), 1);
  EXPECT_EQ(collapsedSpy.count(), 1);
  EXPECT_FALSE(button->isChecked());
}

// ------------------------------------------------------------
// Signals
// ------------------------------------------------------------

TEST(ExpandingWidgetTests, EmitsExactlyOneSignalPerTransition)
{
  qRegisterMetaType<ClickableIcon*>("ClickableIcon*");

  ExpandingWidget widget(Direction::Right);
  widget.setExpandedSize(150);

  auto* button = widget.findChild<ClickableIcon*>();
  ASSERT_NE(button, nullptr);

  QSignalSpy expandedSpy(&widget, &ExpandingWidget::areaExpanded);
  QSignalSpy collapsedSpy(&widget, &ExpandingWidget::areaCollapsed);

  ASSERT_TRUE(expandedSpy.isValid());
  ASSERT_TRUE(collapsedSpy.isValid());

  LOG_INFO("Trigger the expansion");
  ASSERT_TRUE(waitForTransition(widget, true));

  LOG_INFO("Verify that the signal was emitted");
  ASSERT_EQ(expandedSpy.count(), 1);
  EXPECT_EQ(collapsedSpy.count(), 0);
  EXPECT_EQ(expandedSpy.at(0).at(0).value<ClickableIcon*>(), button);

  LOG_INFO("Trigger the collapse");
  ASSERT_TRUE(waitForTransition(widget, false));

  LOG_INFO("Verify that the signal was emitted");
  EXPECT_EQ(expandedSpy.count(), 1);
  ASSERT_EQ(collapsedSpy.count(), 1);
  EXPECT_EQ(collapsedSpy.at(0).at(0).value<ClickableIcon*>(), button);
}
