#include "log_table_model.h"

#include "style_helpers.h"

LogTableModel::LogTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int LogTableModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return mEntries.size();
}

int LogTableModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return ColumnCount;
}

QVariant LogTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return {};

  const auto& entry = mEntries.at(index.row());

  if (index.column() == LevelColumn)
  {
    if (role == Qt::ToolTipRole || role == Qt::UserRole)
      return toString(entry.level);
    else if (role == Qt::DecorationRole)
      return toIcon(entry.level);
    else if (role == Qt::DisplayRole)
      return {};
  }

  if (role == Qt::DisplayRole)
  {
    switch (index.column())
    {
      case TimeColumn:
        return entry.timestamp.toString("HH:mm:ss.zzz");

      case FileColumn:
        return entry.file;

      case LineColumn:
        return entry.line;

      case MessageColumn:
        return entry.message;

      case SourceColumn:
        return entry.source;
    }
  }

  if (role == Qt::BackgroundRole)
  {
    // TODO: Get these from the theme
    if (entry.level == logging::LogLevel::Error)
      return QBrush(QColor(255, 80, 80, 45));

    if (entry.level == logging::LogLevel::Warning)
      return QBrush(QColor(255, 210, 80, 45));
  }

  if (role == Qt::UserRole)
    return static_cast<int>(entry.level);

  return {};
}

QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    return {};

  switch (section)
  {
    case TimeColumn:
      return "Time";
    case LevelColumn:
      return {};
    case FileColumn:
      return "File";
    case LineColumn:
      return "Line";
    case MessageColumn:
      return "Message";
    case SourceColumn:
      return "Source";
  }

  return {};
}

void LogTableModel::append(logging::LogLevel level, const QString& source, const QString& file, const uint32_t line, const QString& message)
{
  const int row = mEntries.size();

  beginInsertRows({}, row, row);
  mEntries.push_back({QDateTime::currentDateTime(),
                      level,
                      file,
                      line,
                      source,
                      message});
  endInsertRows();
}

void LogTableModel::clear()
{
  beginResetModel();
  mEntries.clear();
  endResetModel();
}

QString LogTableModel::toString(logging::LogLevel level) const
{
  switch (level)
  {
    case logging::LogLevel::Trace:
      return "Trace";
    case logging::LogLevel::Debugging:
      return "Debug";
    case logging::LogLevel::Info:
      return "Info";
    case logging::LogLevel::Warning:
      return "Warning";
    case logging::LogLevel::Error:
      return "Error";
  }

  return "Unknown";
}

QIcon LogTableModel::toIcon(logging::LogLevel level) const
{
  switch (level)
  {
    case logging::LogLevel::Trace:
      return addIconWithColor(":/icons/trace.svg", QColor("purple"));
    case logging::LogLevel::Debugging:
      return addIconWithColor(":/icons/debug.svg", QColor("green"));
    case logging::LogLevel::Info:
      return addIconWithColor(":/icons/info.svg", QColor("blue"));
    case logging::LogLevel::Warning:
      return addIconWithColor(":/icons/warning.svg", QColor("yellow"));
    case logging::LogLevel::Error:
      return addIconWithColor(":/icons/error.svg", QColor("red"));
  }

  return QIcon(":/icons/trace.svg");
}