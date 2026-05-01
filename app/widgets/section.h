#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/Common.hpp>

#include "collapsible_area.h"

class ClickableIcon;
class QPropertyAnimation;

class SectionWidget : public QWidget
{
  Q_OBJECT

public:
  SectionWidget(QWidget* parent = nullptr);

  void addItem(QWidget* container, const QString& title);
  void addItem(QWidget* container, const QString& title, oclero::qlementine::TextRole role);
  void setExpanded(bool expanded);
  bool isExpanded() const;

  void setDuration(int duration);

  QWidget* content() const;

  void updateContentHeight(int height);

private:
  ClickableIcon* mToggleButton = nullptr;
  QWidget* mContent = nullptr;
  CollapsibleAreaHeight* mContentArea = nullptr;
  bool mExpanded = true;
  std::optional<int> mDuration = std::nullopt;
  QPropertyAnimation* mAnimation = nullptr;

  inline int getAnimationDuration(int target, int current) const;
  void toggled(bool checked);
};