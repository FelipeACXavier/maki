#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class CollapsibleAreaWidth : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int contentWidth READ contentWidth WRITE setContentWidth)
public:
  CollapsibleAreaWidth(QWidget* parent = nullptr);

  int contentWidth() const;
  void setContentWidth(int width);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

private:
  int mContentWidth;
};

class CollapsibleAreaHeight : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int contentHeight READ contentHeight WRITE setContentHeight)
public:
  CollapsibleAreaHeight(QWidget* parent = nullptr);

  int contentHeight() const;
  void setContentHeight(int height);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

private:
  int mContentHeight;
};