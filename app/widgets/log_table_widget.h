#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "log_filter_model.h"
#include "log_table_highlight.h"
#include "log_table_model.h"
#include "logging.h"

class ClickableIcon;
class ExpandingWidget;

class LogTableWidget : public QWidget
{
  Q_OBJECT
public:
  LogTableWidget(QWidget* parent = nullptr);

  void append(logging::LogLevel level, const QString& source, const QString& file, const uint32_t line, const QString& message);
  void append(logging::LogLevel level, const std::string& source, const std::string& file, const uint32_t line, const std::string& message);
  void clear();

  void search();

private slots:
  void onAreaExpanded(ClickableIcon* button, QLineEdit* lineEdit);
  void onAreaCollapsed(ClickableIcon* button, QLineEdit* lineEdit, const QString& iconName);

  void onClicked(const QModelIndex& index);

private:
  LogTableModel* mModel = nullptr;
  LogFilterProxyModel* mProxy = nullptr;

  QTableView* mTable = nullptr;
  QComboBox* mLevelFilter = nullptr;
  QComboBox* mSourceFilter = nullptr;
  QLineEdit* mFileFilter = nullptr;
  QLineEdit* mSearchField = nullptr;
  QLabel* mSearchCounterLabel = nullptr;

  ExpandingWidget* mSearchBox = nullptr;
  ExpandingWidget* mFilterBox = nullptr;

  LogHighlightDelegate* mHighlightDelegate = nullptr;
  QString mSearchText;
  QVector<QModelIndex> mSearchMatches;

  int mCurrentSearchMatch = -1;
  int mDefaultRowHeight = 24;

  void setSearchText(const QString& text);
  void nextSearchMatch();
  void previousSearchMatch();
  void rebuildSearchMatches();
  void focusCurrentSearchMatch();
  void updateSearchCounter();

  QVector<int> selectedRows() const;

  void showContextMenu(const QPoint& pos);
  void copySelectedRows() const;
  void copySelectedColumn(int column) const;
  void copySelectedColumns(std::initializer_list<int> columns) const;

  bool isMessageElided(int proxyRow) const;

  bool eventFilter(QObject* watched, QEvent* event);
};
