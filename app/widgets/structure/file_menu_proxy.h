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
};