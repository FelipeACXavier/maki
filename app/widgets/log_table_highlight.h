#pragma once

#include <QStyledItemDelegate>

class LogHighlightDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  LogHighlightDelegate(QObject* parent = nullptr);

  void setSearchText(const QString& text);

  void addExpandedRow(int row);
  void removeExpandedRow(int row);
  bool containExpandedRow(int row) const;
  void clearExpandedRows();

  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
  QString mSearchText;
  QVector<int> mExpandedRows = {};
};