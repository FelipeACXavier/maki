#include "expanding_widget.h"

#include "logging.h"

ExpandingWidget::ExpandingWidget(Direction direction, QWidget* parent)
    : QWidget(parent)
    , mExpandedWidth(250)  // Reasonable base value
{
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  mButton = new QPushButton(this);
  mButton->setCheckable(true);
  mButton->setFixedWidth(30);
  mButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  mSearchArea = new CollapsibleAreaWidth(this);
  mSearchArea->setContentWidth(0);

  auto* searchLayout = new QHBoxLayout(mSearchArea);
  searchLayout->setContentsMargins(0, 0, 0, 0);

  mAnimation = new QPropertyAnimation(mSearchArea, "contentWidth", this);
  mAnimation->setDuration(250);
  mAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  if (direction == Direction::Right)
  {
    layout->addWidget(mButton);
    layout->addWidget(mSearchArea);
  }
  else
  {
    layout->addWidget(mSearchArea);
    layout->addWidget(mButton);
  }

  connect(mButton, &QPushButton::toggled, this, &ExpandingWidget::setExpanded);
}

void ExpandingWidget::setExpandedWidth(int width)
{
  mExpandedWidth = width;
}

void ExpandingWidget::setButtonIcon(const QIcon& icon)
{
  mButton->setIcon(icon);
}

void ExpandingWidget::setButtonTooltip(const QString& tooltip)
{
  mButton->setToolTip(tooltip);
  mButton->setToolTipDuration(2000);
}

void ExpandingWidget::addCollapsableWidget(QWidget* widget)
{
  mSearchArea->layout()->addWidget(widget);
}

void ExpandingWidget::setExpanded(bool expanded)
{
  mAnimation->stop();
  mAnimation->setStartValue(mSearchArea->contentWidth());
  mAnimation->setEndValue(expanded ? mExpandedWidth : 0);
  mAnimation->start();

  LOG_DEBUG("%s", expanded ? "Expanding area" : "Collapsing area");
  if (expanded)
    emit areaExpanded(mButton);
  else
    emit areaCollapsed(mButton);
}