#include "log_table_highlight.h"

#include <QPainter>

#include "log_table_model.h"

LogHighlightDelegate::LogHighlightDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void LogHighlightDelegate::setSearchText(const QString& text)
{
  mSearchText = text;
}

void LogHighlightDelegate::addExpandedRow(int row)
{
  mExpandedRows.push_back(row);
}

void LogHighlightDelegate::removeExpandedRow(int row)
{
  mExpandedRows.removeIf([row](int i) { return i == row; });
}

bool LogHighlightDelegate::containExpandedRow(int row) const
{
  return mExpandedRows.contains(row);
}

void LogHighlightDelegate::clearExpandedRows()
{
  mExpandedRows.clear();
}

QSize LogHighlightDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (!containExpandedRow(index.row()) || index.column() != LogTableModel::MessageColumn)
    return QStyledItemDelegate::sizeHint(option, index);

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  const QString text = opt.text;
  const int width = option.rect.width() > 0 ? option.rect.width() : 600;

  QFontMetrics fm(opt.font);

  QRect textRect = fm.boundingRect(QRect(0, 0, width, 10000), Qt::TextWordWrap, text);

  QSize size = QStyledItemDelegate::sizeHint(option, index);
  size.setHeight(textRect.height() + 10);
  return size;
}

void LogHighlightDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  if (containExpandedRow(index.row()) && index.column() == LogTableModel::MessageColumn)
  {
    opt.textElideMode = Qt::ElideNone;
    opt.features |= QStyleOptionViewItem::WrapText;
  }

  QStyledItemDelegate::paint(painter, opt, index);

  if (mSearchText.isEmpty())
    return;

  const QString text = opt.text;
  if (!text.contains(mSearchText, Qt::CaseInsensitive))
    return;

  painter->save();

  QRect textRect = option.widget->style()->subElementRect(QStyle::SE_ItemViewItemText, &opt, option.widget);

  QFontMetrics fm(opt.font);

  const int matchIndex = text.indexOf(mSearchText, 0, Qt::CaseInsensitive);
  const QString before = text.left(matchIndex);
  const QString match = text.mid(matchIndex, mSearchText.length());

  const int x = textRect.x() + fm.horizontalAdvance(before);
  const int w = fm.horizontalAdvance(match);

  QRect highlightRect(x, textRect.y() + 2, w, textRect.height() - 4);

  painter->fillRect(highlightRect, QColor(255, 220, 80, 140));
  painter->restore();
}