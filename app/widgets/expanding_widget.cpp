#include "expanding_widget.h"

#include "clickable_icon.h"
#include "logging.h"
#include "style_helpers.h"

ExpandingWidget::ExpandingWidget(Direction direction, QWidget* parent)
    : QWidget(parent)
    , mExpandedWidth(250)  // Reasonable base value
{
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  mButton = new ClickableIcon(iconFromTheme("arrow-down"), QSize(16, 16), this);
  // mButton = new QPushButton(this);
  mButton->setCheckable(true);
  mButton->setChecked(false);
  mButton->setFixedWidth(30);
  mButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  mSearchArea = new CollapsibleAreaWidth(this);
  mSearchArea->setContentWidth(0);
  mSearchArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

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

  connect(mButton, &ClickableIcon::toggled, this, &ExpandingWidget::setExpanded);
}

void ExpandingWidget::expandArea()
{
  mButton->setChecked(true);
  setExpanded(true);
}

void ExpandingWidget::collapseArea()
{
  mButton->setChecked(false);
  setExpanded(false);
}

void ExpandingWidget::setExpandedWidth(int width)
{
  mExpandedWidth = width;
  mSearchArea->setMaximumWidth(width);
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

  connect(mAnimation, &QPropertyAnimation::finished, this, [this, expanded]() {
    if (expanded)
      emit areaExpanded(mButton);
    else
      emit areaCollapsed(mButton);
  });
}
