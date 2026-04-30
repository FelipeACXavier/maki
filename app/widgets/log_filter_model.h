#pragma once

#include <QSortFilterProxyModel>

class LogFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  LogFilterProxyModel(QObject* parent = nullptr);

  void setLevelFilter(const QString& level);
  void setSourceFilter(const QString& source);
  void setFileFilter(const QString& source);
  void setTextFilter(const QString& text);

protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  QString mLevelFilter = "All";
  QString mSourceFilter;
  QString mFileFilter;
  QString mTextFilter;
};
