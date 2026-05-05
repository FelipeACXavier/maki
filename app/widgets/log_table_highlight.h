#pragma once

#include <QStyledItemDelegate>

/**
 * @brief A delegate for highlighting log entries based on search text and expanding specific rows.
 */
class LogHighlightDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new LogHighlightDelegate object.
   *
   * @param parent The parent object.
   */
  LogHighlightDelegate(QObject* parent = nullptr);

  /**
   * @brief Sets the search text used for highlighting log entries.
   *
   * @param text The search text to set.
   */
  void setSearchText(const QString& text);

  /**
   * @brief Adds a row to the list of expanded rows.
   *
   * @param row The index of the row to add.
   */
  void addExpandedRow(int row);

  /**
   * @brief Removes a row from the list of expanded rows.
   *
   * @param row The index of the row to remove.
   */
  void removeExpandedRow(int row);

  /**
   * @brief Checks if a specific row is in the list of expanded rows.
   *
   * @param row The index of the row to check.
   * @return true If the row is expanded.
   * @return false If the row is not expanded.
   */
  bool containExpandedRow(int row) const;

  /**
   * @brief Clears all expanded rows.
   */
  void clearExpandedRows();

  /**
   * @brief Returns the size hint for a given item view option and model index.
   *
   * @param option The style option for the item view.
   * @param index The model index of the item.
   * @return QSize The size hint for the item.
   */
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  /**
   * @brief Paints an item in a given style option and model index.
   *
   * @param painter The painter to use for drawing.
   * @param option The style option for the item view.
   * @param index The model index of the item.
   */
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
  QString mSearchText; /// The current search text used for highlighting log entries.
  QVector<int> mExpandedRows = {}; /// List of rows that are currently expanded.
  int mMatches = 0; /// Number of matches found during the last search.
};
