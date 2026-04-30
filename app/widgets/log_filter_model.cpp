#include "log_filter_model.h"

#include "log_table_model.h"

LogFilterProxyModel::LogFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
  setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void LogFilterProxyModel::setLevelFilter(const QString& level)
{
  mLevelFilter = level;
  invalidateFilter();
}

void LogFilterProxyModel::setFileFilter(const QString& source)
{
  mFileFilter = source;
  invalidateFilter();
}

void LogFilterProxyModel::setSourceFilter(const QString& source)
{
  mSourceFilter = source;
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
  const QModelIndex sourceIndex = model->index(sourceRow, LogTableModel::FileColumn, sourceParent);
  const QModelIndex messageIndex = model->index(sourceRow, LogTableModel::MessageColumn, sourceParent);
  const QModelIndex fileIndex = model->index(sourceRow, LogTableModel::FileColumn, sourceParent);

  const QString level = model->data(levelIndex, Qt::UserRole).toString();
  const QString source = model->data(sourceIndex, Qt::DisplayRole).toString();
  const QString message = model->data(messageIndex, Qt::DisplayRole).toString();
  const QString file = model->data(fileIndex, Qt::DisplayRole).toString();

  if (mLevelFilter != "All" && level != mLevelFilter)
    return false;

  if (!mFileFilter.isEmpty() && !file.contains(mFileFilter, Qt::CaseInsensitive))
    return false;

  if (!mSourceFilter.isEmpty() && !source.contains(mSourceFilter, Qt::CaseInsensitive))
    return false;

  if (!mTextFilter.isEmpty() && !message.contains(mTextFilter, Qt::CaseInsensitive))
    return false;

  return true;
}
