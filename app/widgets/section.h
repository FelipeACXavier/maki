#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#pragma once

#include <QWidget>

class CollapsibleArea : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int contentHeight READ contentHeight WRITE setContentHeight)
public:
  explicit CollapsibleArea(QWidget* parent = nullptr)
      : QWidget(parent)
      , mContentHeight(0)
  {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  }

  int contentHeight() const
  {
    return mContentHeight;
  }

  void setContentHeight(int height)
  {
    if (mContentHeight == height)
      return;

    mContentHeight = height;
    updateGeometry();
    resize(width(), mContentHeight);
  }

  QSize sizeHint() const override
  {
    QSize s = QWidget::sizeHint();
    s.setHeight(mContentHeight);
    return s;
  }

  QSize minimumSizeHint() const override
  {
    QSize s = QWidget::minimumSizeHint();
    s.setHeight(mContentHeight);
    return s;
  }

private:
  int mContentHeight;
};

class SectionWidget : public QWidget
{
  Q_OBJECT

public:
  SectionWidget(QWidget* parent = nullptr);

  void addItem(QWidget* content, const QString& title);
  void setExpanded(bool expanded);
  bool isExpanded() const;

  QWidget* content() const;

  void updateContentHeight(int height);

private:
  QToolButton* mToggleButton = nullptr;
  QWidget* mContent = nullptr;
  CollapsibleArea* mContentArea = nullptr;

  inline int getAnimationDuration(int target, int current) const;
};