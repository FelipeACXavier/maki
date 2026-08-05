#include "log_filter_model.h"

#include "log_table_model.h"

LogFilterProxyModel::LogFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
  setFilterCaseSensitivity(Qt::CaseInsensitive);

  for (int value = static_cast<int>(logging::LogLevel::Error); value <= static_cast<int>(logging::gMinLogLevel); ++value)
    mEnabledLevels.insert(static_cast<logging::LogLevel>(value));
}

void LogFilterProxyModel::setLevelEnabled(logging::LogLevel level, bool enabled)
{
  if (enabled)
    mEnabledLevels.insert(level);
  else
    mEnabledLevels.remove(level);

  invalidateFilter();
}

void LogFilterProxyModel::setFileFilter(const QString& source)
{
  mFileFilter = source;
  invalidateFilter();
}

void LogFilterProxyModel::setSourceEnabled(const QString& source, bool enabled)
{
  if (enabled)
    mSourcesLevels.insert(source);
  else
    mSourcesLevels.remove(source);

  invalidateFilter();
}

void LogFilterProxyModel::setTextFilter(const QString& text)
{
  mTextFilter = text;
  invalidateFilter();
}

bool LogFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  const auto* model = sourceModel();

  const QModelIndex levelIndex = model->index(sourceRow, LogTableModel::LevelColumn, sourceParent);
  const QModelIndex sourceIndex = model->index(sourceRow, LogTableModel::SourceColumn, sourceParent);
  const QModelIndex messageIndex = model->index(sourceRow, LogTableModel::MessageColumn, sourceParent);
  const QModelIndex fileIndex = model->index(sourceRow, LogTableModel::FileColumn, sourceParent);

  const logging::LogLevel level = static_cast<logging::LogLevel>(model->data(levelIndex, Qt::UserRole).toInt());
  const QString source = model->data(sourceIndex, Qt::DisplayRole).toString();
  const QString message = model->data(messageIndex, Qt::DisplayRole).toString();
  const QString file = model->data(fileIndex, Qt::DisplayRole).toString();

  if (!mEnabledLevels.contains(level))
    return false;

  if (!mFileFilter.isEmpty() && !file.contains(mFileFilter, Qt::CaseInsensitive))
    return false;

  if (!mSourcesLevels.contains(source))
    return false;

  if (!mTextFilter.isEmpty() && !message.contains(mTextFilter, Qt::CaseInsensitive))
    return false;

  return true;
}
