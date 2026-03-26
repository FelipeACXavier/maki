#pragma once

#include <QFileInfo>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

class FileTypeProxyModel : public QSortFilterProxyModel
{
public:
  using QSortFilterProxyModel::QSortFilterProxyModel;

  QVariant data(const QModelIndex& index, int role) const override
  {
    if (role == Qt::DisplayRole && index.column() == 2)  // Type column
    {
      auto* fsModel = qobject_cast<QFileSystemModel*>(sourceModel());
      QModelIndex sourceIndex = mapToSource(index);

      QFileInfo info = fsModel->fileInfo(sourceIndex);

      if (info.isDir())
        return "dir";

      const QString ext = info.suffix().toLower();
      return ext;
    }

    return QSortFilterProxyModel::data(index, role);
  }

  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
  {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid())
      return false;

    auto* fsModel = qobject_cast<QFileSystemModel*>(sourceModel());
    if (!fsModel)
      return true;

    QFileInfo info = fsModel->fileInfo(index);

    // Always keep directories visible so the user can navigate
    if (info.isDir())
      return true;

    return mAllowedExtensions.contains(info.suffix().toLower());
  }

private:
  QSet<QString> mAllowedExtensions = {
      "h",
      "hh",
      "c",
      "cpp",
      "txt",
      "py",
      "cc",
      "dzn"};
};