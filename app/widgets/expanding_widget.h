#pragma once

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QWidget>

#include "collapsible_area.h"

class ExpandingWidget : public QWidget
{
  Q_OBJECT

public:
  enum class Direction
  {
    Right,
    Left
  };

  ExpandingWidget(Direction direction, QWidget* parent = nullptr);

  void setButtonIcon(const QIcon& icon);
  void setButtonTooltip(const QString& tooltip);

  void addCollapsableWidget(QWidget* widget);
  void setExpandedWidth(int width);

signals:
  void textChanged(const QString& text);
  void areaExpanded(QPushButton* button);
  void areaCollapsed(QPushButton* button);

public slots:
  void setExpanded(bool expanded);

private:
  QPushButton* mButton = nullptr;
  CollapsibleAreaWidth* mSearchArea = nullptr;
  QPropertyAnimation* mAnimation = nullptr;

  int mExpandedWidth;
};