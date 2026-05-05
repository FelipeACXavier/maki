#pragma once

#include <QAbstractTableModel>
#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include "logging.h"

/**
 * @brief A custom table model for displaying log entries.
 */
class LogTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  enum Column
  {
    LevelColumn,  /// The column index for the log level.
    TimeColumn,   /// The column index for the timestamp.
    MessageColumn,/// The column index for the log message.
    SourceColumn, /// The column index for the source of the log entry.
    FileColumn,   /// The column index for the file where the log entry was generated.
    LineColumn,   /// The column index for the line number in the file.
    ColumnCount   /// The total number of columns.
  };

  struct LogEntry
  {
    QDateTime timestamp;  /// The timestamp of the log entry.
    logging::LogLevel level;  /// The log level of the entry.
    QString file;         /// The file where the log entry was generated.
    uint32_t line;        /// The line number in the file.
    QString source;       /// The source of the log entry.
    QString message;      /// The log message.
  };

  /**
   * @brief Constructs a new LogTableModel object.
   *
   * @param parent The parent object.
   */
  LogTableModel(QObject* parent = nullptr);

  /**
   * @brief Returns the number of rows in the model.
   *
   * @param parent The parent index.
   * @return int The number of rows.
   */
  int rowCount(const QModelIndex& parent = {}) const override;

  /**
   * @brief Returns the number of columns in the model.
   *
   * @param parent The parent index.
   * @return int The number of columns.
   */
  int columnCount(const QModelIndex& parent = {}) const override;

  /**
   * @brief Returns the data for a given role and index.
   *
   * @param index The index of the item to retrieve.
   * @param role The role of the data to retrieve.
   * @return QVariant The data for the specified role and index.
   */
  QVariant data(const QModelIndex& index, int role) const override;

  /**
   * @brief Returns the header data for a given section and orientation.
   *
   * @param section The section of the header.
   * @param orientation The orientation of the header (horizontal or vertical).
   * @param role The role of the data to retrieve.
   * @return QVariant The header data for the specified section, orientation, and role.
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  /**
   * @brief Appends a new log entry to the model.
   *
   * @param level The log level of the entry.
   * @param source The source of the log entry.
   * @param file The file where the log entry was generated.
   * @param line The line number in the file.
   * @param message The log message.
   */
  void append(logging::LogLevel level, const QString& source, const QString& file, const uint32_t line, const QString& message);

  /**
   * @brief Clears all log entries from the model.
   */
  void clear();

  /**
   * @brief Converts a log level to a string representation.
   *
   * @param level The log level to convert.
   * @return QString The string representation of the log level.
   */
  QString toString(logging::LogLevel level) const;

  /**
   * @brief Converts a log level to an icon.
   *
   * @param level The log level to convert.
   * @return QIcon The icon representing the log level.
   */
  QIcon toIcon(logging::LogLevel level) const;

private:
  QVector<LogEntry> mEntries;  /// The vector containing all log entries.
};
