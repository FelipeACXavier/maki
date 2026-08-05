#include "log_table_highlight.h"

#include <QPainter>
#include <QTextLine>
#include <oclero/qlementine.hpp>

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

void LogHighlightDelegate::updatePadding(int hPadding, int vPadding)
{
  mExpandedHPadding = hPadding;
  mExpandedHPadding = vPadding;
}

QSize LogHighlightDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  const bool expandedMessage = containExpandedRow(index.row()) && index.column() == LogTableModel::MessageColumn;

  if (!expandedMessage)
    return size;

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  QStyle* style = opt.widget ? opt.widget->style() : oclero::qlementine::appStyle();
  QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);

  int availableWidth = textRect.width();

  // sizeHint() may occasionally receive an option without a useful rect.
  if (availableWidth <= 0)
    availableWidth = option.rect.width();

  if (availableWidth <= 0)
    availableWidth = 600;

  availableWidth -= 2 * mExpandedHPadding;
  availableWidth = std::max(1, availableWidth);

  const QFontMetrics fontMetrics(opt.font);

  const QRect wrappedRect = fontMetrics.boundingRect(
      QRect(0, 0, availableWidth, std::numeric_limits<int>::max()),
      Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
      opt.text);

  const int requiredHeight = wrappedRect.height() + 2 * mExpandedVPadding + 2;  // Safety margin for font/device-pixel rounding.
  size.setHeight(std::max(size.height(), requiredHeight));
  return size;
}

void LogHighlightDelegate::paintSearchHighlights(QPainter* painter, const QStyleOptionViewItem& option, const QRect& textRect, const QString& text, bool wordWrap) const
{
  if (mSearchText.isEmpty() || text.isEmpty() || !text.contains(mSearchText, Qt::CaseInsensitive) || textRect.width() <= 0)
    return;

  const auto* qlementinestyle = oclero::qlementine::appStyle();
  if (!qlementinestyle)
    return;

  QTextLayout layout(text, option.font);
  QTextOption textOption;
  textOption.setWrapMode(wordWrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
  layout.setTextOption(textOption);

  layout.beginLayout();
  qreal y = 0.0;
  while (true)
  {
    QTextLine line = layout.createLine();
    if (!line.isValid())
      break;

    line.setLineWidth(textRect.width());
    line.setPosition(QPointF(0.0, y));
    y += line.height();

    // No need to create more lines if we are not wrapping
    if (!wordWrap)
      break;
  }
  layout.endLayout();

  const qreal layoutHeight = y;
  qreal verticalOffset = 0.0;
  if (option.displayAlignment.testFlag(Qt::AlignVCenter))
    verticalOffset = std::max<qreal>(0.0, (textRect.height() - layoutHeight) / 2.0);
  else if (option.displayAlignment.testFlag(Qt::AlignBottom))
    verticalOffset = std::max<qreal>(0.0, textRect.height() - layoutHeight);

  painter->save();

  // Find all the matches in the text
  int searchPosition = 0;
  while (searchPosition < text.length())
  {
    const int matchStart = text.indexOf(mSearchText, searchPosition, Qt::CaseInsensitive);
    if (matchStart < 0)
      break;

    const int matchEnd = matchStart + mSearchText.length();
    for (int lineIndex = 0; lineIndex < layout.lineCount(); ++lineIndex)
    {
      const QTextLine line = layout.lineAt(lineIndex);
      const int lineStart = line.textStart();
      const int lineEnd = lineStart + line.textLength();
      const int segmentStart = std::max(matchStart, lineStart);
      const int segmentEnd = std::min(matchEnd, lineEnd);

      if (segmentStart >= segmentEnd)
        continue;

      qreal startX = line.cursorToX(segmentStart);
      qreal endX = line.cursorToX(segmentEnd);

      if (endX < startX)
        std::swap(startX, endX);

      QRectF highlightRect(
          textRect.left() + line.position().x() + startX,
          textRect.top() + verticalOffset + line.position().y(),
          endX - startX,
          line.height());

      painter->fillRect(highlightRect, qlementinestyle->theme().statusColorWarning);
    }

    // Move beyond this match, so we dont get stuck on the same match forever
    searchPosition = matchEnd;
  }

  painter->restore();
}

void LogHighlightDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  const auto* style = opt.widget ? opt.widget->style() : oclero::qlementine::appStyle();

  QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
  textRect.adjust(mExpandedHPadding, mExpandedVPadding, -mExpandedHPadding, -mExpandedVPadding);

  const bool isExpanded = containExpandedRow(index.row()) && index.column() == LogTableModel::MessageColumn;
  const auto flags = isExpanded ? (Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap) : (Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine);
  const QString displayedText = isExpanded ? opt.text : opt.fontMetrics.elidedText(opt.text, opt.textElideMode, textRect.width());

  // Draw the standard Qlementine background, selection, icon, etc., but suppress the default text so that
  // we can draw the search highlights
  QStyleOptionViewItem backgroundOption(opt);
  backgroundOption.text.clear();

  style->drawControl(QStyle::CE_ItemViewItem, &backgroundOption, painter, opt.widget);

  // Paint the search highlights behind the text.
  if (isExpanded)
  {
    QStyleOptionViewItem textOption(opt);
    textOption.text = displayedText;
    textOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    paintSearchHighlights(painter, textOption, textRect, opt.text, isExpanded);
  }
  else
  {
    paintSearchHighlights(painter, opt, textRect, opt.text, isExpanded);
  }

  // Move on to draw the text
  painter->save();
  painter->setPen(opt.state.testFlag(QStyle::State_Selected) ? opt.palette.color(QPalette::HighlightedText) : opt.palette.color(QPalette::Text));
  painter->drawText(textRect, flags, displayedText);
  painter->restore();
}