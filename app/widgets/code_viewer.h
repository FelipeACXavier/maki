#pragma once

#include <QPlainTextEdit>

class LineNumberArea;

class CodeViewer : public QPlainTextEdit
{
  Q_OBJECT

public:
  CodeViewer(QWidget* parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent* event);
  int lineNumberAreaWidth() const;

protected:
  void resizeEvent(QResizeEvent* event) override;

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect& rect, int dy);

private:
  LineNumberArea* mLineNumberArea = nullptr;
};

class LineNumberArea : public QWidget
{
public:
  LineNumberArea(CodeViewer* editor)
      : QWidget(editor)
      , mEditor(editor)
  {
  }

  QSize sizeHint() const override
  {
    return QSize(mEditor->lineNumberAreaWidth(), 0);
  }

protected:
  void paintEvent(QPaintEvent* event) override
  {
    mEditor->lineNumberAreaPaintEvent(event);
  }

private:
  CodeViewer* mEditor = nullptr;
};