#pragma once

#include <QPushButton>
#include <QWidget>

#include "log_filter_model.h"
#include "log_table_highlight.h"
#include "log_table_model.h"
#include "logging.h"

class LogTableWidget : public QWidget
{
  Q_OBJECT

public:
  LogTableWidget(QWidget* parent = nullptr);

  void append(logging::LogLevel level, const QString& file, const uint32_t line, const QString& message);
  void append(logging::LogLevel level, const std::string& file, const uint32_t line, const std::string& message);
  void clear();

private slots:
  void onAreaExpanded(QPushButton* button, QLineEdit* lineEdit);
  void onAreaCollapsed(QPushButton* button, QLineEdit* lineEdit, const QString& iconName);

  void onClicked(const QModelIndex& index);

private:
  LogTableModel* mModel = nullptr;
  LogFilterProxyModel* mProxy = nullptr;

  QTableView* mTable = nullptr;
  QComboBox* mLevelFilter = nullptr;
  QComboBox* mSourceFilter = nullptr;
  QLineEdit* mFileFilter = nullptr;
  QLineEdit* mSearchField = nullptr;

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

  QVector<int> selectedRows() const;

  void showContextMenu(const QPoint& pos);
  void copySelectedRows() const;
  void copySelectedColumn(int column) const;
  void copySelectedColumns(std::initializer_list<int> columns) const;

  bool isMessageElided(int proxyRow) const;
};