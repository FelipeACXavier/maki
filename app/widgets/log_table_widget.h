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

/**
 * @brief A widget for displaying log tables with filtering and search functionality.
 */
class LogTableWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new LogTableWidget.
   *
   * @param parent The parent widget.
   */
  LogTableWidget(QWidget* parent = nullptr);

  /**
   * @brief Appends a log message with a QString source and file.
   *
   * @param level The log level.
   * @param source The source of the log message.
   * @param file The file where the log message was generated.
   * @param line The line number in the file.
   * @param message The log message.
   */
  void append(logging::LogLevel level, const QString& source, const QString& file, const uint32_t line, const QString& message);

  /**
   * @brief Appends a log message with a std::string source and file.
   *
   * @param level The log level.
   * @param source The source of the log message.
   * @param file The file where the log message was generated.
   * @param line The line number in the file.
   * @param message The log message.
   */
  void append(logging::LogLevel level, const std::string& source, const std::string& file, const uint32_t line, const std::string& message);

  /**
   * @brief Clears all log messages from the table.
   */
  void clear();

  /**
   * @brief Initiates a search for the current search text.
   */
  void search();

  /**
   * @brief Expand all the expandable widgets
   */
  void showAll();

private slots:
  /**
   * @brief Handles area expansion events.
   *
   * @param button The button that triggered the event.
   * @param lineEdit The line edit associated with the button.
   */
  void onAreaExpanded(ClickableIcon* button, QLineEdit* lineEdit);

  /**
   * @brief Handles area collapse events.
   *
   * @param button The button that triggered the event.
   * @param lineEdit The line edit associated with the button.
   * @param iconName The icon name for the collapsed state.
   */
  void onAreaCollapsed(ClickableIcon* button, QLineEdit* lineEdit, const QString& iconName);

  /**
   * @brief Handles click events on table items.
   *
   * @param index The model index of the clicked item.
   */
  void onClicked(const QModelIndex& index);

private:
  LogTableModel* mModel = nullptr;        /// Pointer to the log table model.
  LogFilterProxyModel* mProxy = nullptr;  /// Pointer to the log filter proxy model.

  QTableView* mTable = nullptr;           /// Pointer to the table view widget.
  QToolButton* mLevelFilter = nullptr;    /// Pointer to the level filter combo box.
  QToolButton* mSourceFilter = nullptr;   /// Pointer to the source filter combo box.
  QLineEdit* mFileFilter = nullptr;       /// Pointer to the file filter line edit.
  QLineEdit* mSearchField = nullptr;      /// Pointer to the search field line edit.
  QLabel* mSearchCounterLabel = nullptr;  /// Pointer to the search counter label.

  ExpandingWidget* mSearchBox = nullptr;  /// Pointer to the search box widget.
  ExpandingWidget* mFilterBox = nullptr;  /// Pointer to the filter box widget.

  LogHighlightDelegate* mHighlightDelegate = nullptr;  /// Pointer to the log highlight delegate.
  QString mSearchText;                                 /// The current search text.
  QVector<QModelIndex> mSearchMatches;                 /// List of search matches.
  QSet<QString> mSources;                              /// Set of sources for filtering.

  int mCurrentSearchMatch = -1;  /// Index of the current search match.
  int mDefaultRowHeight = 24;    /// Default row height in pixels.

  /**
   * @brief Sets the search text and updates the search matches.
   *
   * @param text The new search text.
   */
  void setSearchText(const QString& text);

  /**
   * @brief Moves to the next search match.
   */
  void nextSearchMatch();

  /**
   * @brief Moves to the previous search match.
   */
  void previousSearchMatch();

  /**
   * @brief Rebuilds the list of search matches.
   */
  void rebuildSearchMatches();

  /**
   * @brief Focuses on the current search match.
   */
  void focusCurrentSearchMatch();

  /**
   * @brief Updates the search counter label.
   */
  void updateSearchCounter();

  /**
   * @brief Returns a vector of selected row indices.
   *
   * @return A vector of selected row indices.
   */
  QVector<int> selectedRows() const;

  /**
   * @brief Shows the context menu at the specified position.
   *
   * @param pos The position where the context menu should be shown.
   */
  void showContextMenu(const QPoint& pos);

  /**
   * @brief Copies the selected rows to the clipboard.
   */
  void copySelectedRows() const;

  /**
   * @brief Copies a specific column of the selected rows to the clipboard.
   *
   * @param column The index of the column to copy.
   */
  void copySelectedColumn(int column) const;

  /**
   * @brief Copies multiple columns of the selected rows to the clipboard.
   *
   * @param columns A list of column indices to copy.
   */
  void copySelectedColumns(std::initializer_list<int> columns) const;

  /**
   * @brief Checks if a message is elided in the table.
   *
   * @param proxyRow The row index in the proxy model.
   * @return True if the message is elided, false otherwise.
   */
  bool isMessageElided(int proxyRow) const;

  /**
   * @brief Filters events for child widgets.
   *
   * @param watched The widget being watched.
   * @param event The event to filter.
   * @return True if the event should be accepted, false otherwise.
   */
  bool eventFilter(QObject* watched, QEvent* event);

  /**
   * @brief Updates the text in a toolbutton filter.
   *
   * @param button The toolbutton to be updated.
   * @param base The default text.
   */
  void updateFilterText(QToolButton* button, const QString& base);
};
