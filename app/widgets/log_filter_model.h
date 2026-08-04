#pragma once

#include <QSortFilterProxyModel>

/**
 * @brief A proxy model for filtering log entries based on various criteria.
 */
class LogFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new LogFilterProxyModel with the given parent object.
   *
   * @param parent The parent object.
   */
  LogFilterProxyModel(QObject* parent = nullptr);

  /**
   * @brief Sets the filter for log levels.
   *
   * @param level The log level to filter by. Default is "All".
   */
  void setLevelFilter(const QString& level);

  /**
   * @brief Sets the filter for source names.
   *
   * @param source The source name to filter by.
   */
  void setSourceFilter(const QString& source);

  /**
   * @brief Sets the filter for file names.
   *
   * @param source The file name to filter by.
   */
  void setFileFilter(const QString& source);

  /**
   * @brief Sets the text filter for log entries.
   *
   * @param text The text to filter by.
   */
  void setTextFilter(const QString& text);

protected:
  /**
   * @brief Determines whether a row should be accepted based on the current filters.
   *
   * @param sourceRow The index of the row in the source model.
   * @param sourceParent The parent index of the row in the source model.
   * @return true if the row should be accepted, false otherwise.
   */
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  QString mLevelFilter = "All";   /// The current log level filter.
  QString mSourceFilter = "All";  /// The current source name filter.
  QString mFileFilter;            /// The current file name filter.
  QString mTextFilter;            /// The current text filter.
};
