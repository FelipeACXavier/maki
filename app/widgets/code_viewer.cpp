#include "code_viewer.h"

#include <QPainter>
#include <QTextBlock>

CodeViewer::CodeViewer(QWidget* parent)
    : QPlainTextEdit(parent)
{
  mLineNumberArea = new LineNumberArea(this);

  connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeViewer::updateLineNumberAreaWidth);
  connect(this, &QPlainTextEdit::updateRequest, this, &CodeViewer::updateLineNumberArea);
  connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeViewer::highlightCurrentLine);

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}

int CodeViewer::lineNumberAreaWidth() const
{
  int digits = 1;
  int max = qMax(1, blockCount());

  while (max >= 10)
  {
    max /= 10;
    ++digits;
  }

  const int space = 8 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

  return space;
}

void CodeViewer::updateLineNumberAreaWidth(int)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeViewer::updateLineNumberArea(const QRect& rect, int dy)
{
  if (dy)
    mLineNumberArea->scroll(0, dy);
  else
    mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}

void CodeViewer::resizeEvent(QResizeEvent* event)
{
  QPlainTextEdit::resizeEvent(event);

  const QRect cr = contentsRect();

  mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeViewer::lineNumberAreaPaintEvent(QPaintEvent* event)
{
  QPainter painter(mLineNumberArea);

  painter.fillRect(event->rect(), palette().window());

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();

  int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());

  int bottom = top + qRound(blockBoundingRect(block).height());

  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      const QString number = QString::number(blockNumber + 1);

      painter.setPen(palette().text().color());

      painter.drawText(0, top, mLineNumberArea->width() - 4, fontMetrics().height(), Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + qRound(blockBoundingRect(block).height());

    ++blockNumber;
  }
}

void CodeViewer::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = palette().alternateBase().color();

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);

    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}