#include <gtest/gtest.h>

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSignalSpy>
#include <QTableView>
#include <QTest>

#include "log_table_widget.h"

class LogTableWidgetTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    widget = std::make_unique<LogTableWidget>();
    widget->resize(900, 400);
    widget->show();

    bool waited = QTest::qWaitForWindowExposed(widget.get(), 2000);
    if (!waited)
      FAIL() << "Waited too long for the widget to appear";

    table = widget->findChild<QTableView*>();
    ASSERT_NE(table, nullptr);
  }

  void appendInfo(const QString& source = "test", const QString& file = "test.cpp",
                  uint32_t line = 42, const QString& message = "hello world")
  {
    widget->append(logging::LogLevel::Info, source, file, line, message);
    QApplication::processEvents();
  }

  QLineEdit* searchField() const
  {
    return widget->findChild<QLineEdit*>("SearchField");
  }

  QLineEdit* fileFilter() const
  {
    return widget->findChild<QLineEdit*>("FileFilter");
  }

  QComboBox* sourceFilter() const
  {
    return widget->findChild<QComboBox*>("SourceFilter");
  }

  std::unique_ptr<LogTableWidget> widget;
  QTableView* table = nullptr;
};

TEST_F(LogTableWidgetTest, StartsWithEmptyTable)
{
  ASSERT_NE(table->model(), nullptr);
  EXPECT_EQ(table->model()->rowCount(), 0);
}

TEST_F(LogTableWidgetTest, AppendAddsRowToTable)
{
  for (int i = 0; i < 10; ++i)
    appendInfo();

  EXPECT_EQ(table->model()->rowCount(), 10);
}

TEST_F(LogTableWidgetTest, AppendCleansNewlinesFromMessage)
{
  appendInfo("test", "test.cpp", 42, "hello\nworld\r\nagain");

  ASSERT_EQ(table->model()->rowCount(), 1);

  const QModelIndex index = table->model()->index(0, LogTableModel::MessageColumn);
  EXPECT_EQ(index.data(Qt::DisplayRole).toString(), "hello world again");
}

TEST_F(LogTableWidgetTest, ClearRemovesRows)
{
  appendInfo();
  appendInfo("test", "other.cpp", 43, "second message");

  ASSERT_EQ(table->model()->rowCount(), 2);

  widget->clear();
  QApplication::processEvents();

  EXPECT_EQ(table->model()->rowCount(), 0);
}

TEST_F(LogTableWidgetTest, AppendAddsNewSourceToSourceFilter)
{
  appendInfo("navigation", "nav.cpp", 12, "started navigation");

  auto* source = sourceFilter();
  ASSERT_NE(source, nullptr);

  EXPECT_GE(source->findText("navigation"), 0);
}

TEST_F(LogTableWidgetTest, FileFilterFiltersVisibleRows)
{
  appendInfo("test", "navigation.cpp", 12, "navigation message");
  appendInfo("test", "vision.cpp", 20, "vision message");

  ASSERT_EQ(table->model()->rowCount(), 2);

  auto* filter = fileFilter();
  ASSERT_NE(filter, nullptr);

  filter->setText("vision");
  QApplication::processEvents();

  EXPECT_EQ(table->model()->rowCount(), 1);

  const QModelIndex fileIndex =
      table->model()->index(0, LogTableModel::FileColumn);

  EXPECT_EQ(fileIndex.data(Qt::DisplayRole).toString(), "vision.cpp");
}

TEST_F(LogTableWidgetTest, SearchSelectsMatchingRow)
{
  appendInfo("test", "navigation.cpp", 12, "robot is moving");
  appendInfo("test", "vision.cpp", 20, "object detected");

  auto* search = searchField();
  ASSERT_NE(search, nullptr);

  search->setText("object");
  QApplication::processEvents();

  const QModelIndex current = table->currentIndex();

  ASSERT_TRUE(current.isValid());
  EXPECT_EQ(current.row(), 1);
}

TEST_F(LogTableWidgetTest, EnterMovesToNextSearchMatch)
{
  for (int i = 0; i < 10; ++i)
    appendInfo("test", "a.cpp", i, "same message one");

  auto* search = searchField();
  ASSERT_NE(search, nullptr);

  search->setFocus();
  search->setText("same");
  QApplication::processEvents();

  int firstRow = table->currentIndex().row();
  for (int i = 0; i < 10; ++i)
  {
    const int lineData = table->model()->index(firstRow, LogTableModel::LineColumn).data(Qt::DisplayRole).toInt();
    EXPECT_EQ(lineData, i);

    QTest::keyClick(search, Qt::Key_Return);
    QApplication::processEvents();
    const int currentRow = table->currentIndex().row();
    EXPECT_NE(currentRow, firstRow);
    firstRow = currentRow;
  }
}