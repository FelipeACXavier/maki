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

class LogTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  enum Column
  {
    LevelColumn,
    TimeColumn,
    MessageColumn,
    SourceColumn,
    FileColumn,
    LineColumn,
    ColumnCount
  };

  struct LogEntry
  {
    QDateTime timestamp;
    logging::LogLevel level;
    QString file;
    uint32_t line;
    QString source;
    QString message;
  };

  LogTableModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;

  QVariant data(const QModelIndex& index, int role) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  void append(logging::LogLevel level, const QString& source, const QString& file, const uint32_t line, const QString& message);
  void clear();

  QString toString(logging::LogLevel level) const;
  QIcon toIcon(logging::LogLevel level) const;

private:
  QVector<LogEntry> mEntries;
};
