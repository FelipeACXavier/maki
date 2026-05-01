#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "collapsible_area.h"

class ClickableIcon;

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
  ClickableIcon* mToggleButton = nullptr;
  QWidget* mContent = nullptr;
  CollapsibleAreaHeight* mContentArea = nullptr;
  bool mExpanded = true;

  inline int getAnimationDuration(int target, int current) const;
};