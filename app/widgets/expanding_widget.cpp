#include "expanding_widget.h"

#include <oclero/qlementine/widgets/Label.hpp>

#include "app_configs.h"
#include "clickable_icon.h"

ExpandingWidget::ExpandingWidget(Direction direction, QWidget* parent)
    : QWidget(parent)
    , mButton(nullptr)
    , mSearchArea(nullptr)
    , mAnimation(nullptr)
    , mDirection(direction)
    , mExpandedSize(250)
{
  if (direction == Direction::Up)
    setupVerticalWidget();
  else
    setupHorizontalWidget();

  assert(mSearchArea != nullptr);
  assert(mContentWidget != nullptr);
  assert(mAnimation != nullptr);

  mContentWidget->move(0, 0);

  mAnimation->setDuration(250);
  mAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void ExpandingWidget::setupVerticalWidget()
{
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  mSearchArea = new CollapsibleAreaHeight(this);
  mSearchArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  qobject_cast<CollapsibleAreaHeight*>(mSearchArea)->setContentHeight(0);

  mContentWidget = new QWidget(mSearchArea);
  mContentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto* contentLayout = new QVBoxLayout(mContentWidget);
  contentLayout->setContentsMargins(0, 0, 0, 0);

  mAnimation = new QPropertyAnimation(mSearchArea, "contentHeight", this);

  // Add the widgets
  layout->addWidget(mSearchArea);
}

void ExpandingWidget::setupHorizontalWidget()
{
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  mButton = new ClickableIcon(QIcon(":/icons/arrow-down.svg"), Config::SMALL_BUTTON_SIZE, this);

  mButton->setCheckable(true);
  mButton->setChecked(false);
  mButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  mButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  auto* widthArea = new CollapsibleAreaWidth(this);

  widthArea->setContentWidth(0);
  widthArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  mSearchArea = widthArea;

  mContentWidget = new QWidget(mSearchArea);
  mContentWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto* contentLayout = new QHBoxLayout(mContentWidget);
  contentLayout->setContentsMargins(0, 0, 0, 0);

  mAnimation = new QPropertyAnimation(mSearchArea, "contentWidth", this);

  if (mDirection == Direction::Right)
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
  if (mButton)
    mButton->setChecked(true);

  setExpanded(true);
}

void ExpandingWidget::collapseArea()
{
  if (mButton)
    mButton->setChecked(false);

  setExpanded(false);
}

void ExpandingWidget::setExpandedSize(int size)
{
  mExpandedSize = std::max(0, size);

  if (mDirection == Direction::Up)
    qobject_cast<CollapsibleAreaHeight*>(mSearchArea)->setMaximumHeight(mExpandedSize);
  else
    qobject_cast<CollapsibleAreaWidth*>(mSearchArea)->setMaximumWidth(mExpandedSize);

  updateCrossAxisSize();
  updateContentGeometry();
}

void ExpandingWidget::setButtonIcon(const QIcon& icon)
{
  if (mButton)
    mButton->setIcon(icon);
}

void ExpandingWidget::setButtonTooltip(const QString& tooltip)
{
  if (!mButton)
    return;

  mButton->setToolTip(tooltip);
  mButton->setToolTipDuration(2000);
}

void ExpandingWidget::addCollapsableWidget(QWidget* widget, const QString& label)
{
  if (!widget || !mContentWidget || !mContentWidget->layout())
    return;

  if (!label.isEmpty())
  {
    auto* labelWidget = new oclero::qlementine::Label(label, mContentWidget);
    mContentWidget->layout()->addWidget(labelWidget);
  }

  mContentWidget->layout()->addWidget(widget);
  mContentWidget->layout()->invalidate();
  mContentWidget->layout()->activate();

  updateCrossAxisSize();
  updateContentGeometry();
}

void ExpandingWidget::setExpanded(bool expanded)
{
  if (!mAnimation || !mSearchArea)
    return;

  mAnimation->stop();
  updateContentGeometry();

  if (mDirection == Direction::Up)
  {
    auto* area = qobject_cast<CollapsibleAreaHeight*>(mSearchArea);
    mAnimation->setStartValue(area->contentHeight());
    mAnimation->setEndValue(expanded ? mExpandedSize : 0);
  }
  else
  {
    auto* area = qobject_cast<CollapsibleAreaWidth*>(mSearchArea);
    mAnimation->setStartValue(area->contentWidth());
    mAnimation->setEndValue(expanded ? mExpandedSize : 0);
  }

  connect(mAnimation, &QPropertyAnimation::finished, this, [this, expanded] {
    if (expanded)
      emit areaExpanded(mButton);
    else
      emit areaCollapsed(mButton);
  });

  mAnimation->start();
}

QVBoxLayout* ExpandingWidget::getContent() const
{
  if (mDirection != Direction::Up || !mContentWidget)
    return nullptr;

  return qobject_cast<QVBoxLayout*>(mContentWidget->layout());
}

void ExpandingWidget::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  updateContentGeometry();
}

void ExpandingWidget::updateCrossAxisSize()
{
  if (!mSearchArea || !mContentWidget || !mContentWidget->layout())
    return;

  // Width is controlled by the parent layout.
  if (mDirection == Direction::Up)
    return;

  auto* contentLayout = mContentWidget->layout();
  contentLayout->invalidate();
  contentLayout->activate();

  const int contentHeight = contentLayout->sizeHint().height();
  const int buttonHeight = mButton ? mButton->sizeHint().height() : 0;

  const int requiredHeight = std::max(contentHeight, buttonHeight);

  mSearchArea->setFixedHeight(requiredHeight);
  mContentWidget->setFixedHeight(requiredHeight);
  setFixedHeight(requiredHeight);

  updateGeometry();
}

void ExpandingWidget::updateContentGeometry()
{
  if (!mSearchArea || !mContentWidget || !mContentWidget->layout())
    return;

  auto* contentLayout = mContentWidget->layout();
  contentLayout->invalidate();

  if (mDirection == Direction::Up)
  {
    // Width follows the clipping viewport.
    const int contentWidth = mSearchArea->width();
    if (contentWidth <= 0)
      return;

    // Height remains at the complete expanded size.
    mContentWidget->setGeometry(0, 0, contentWidth, mExpandedSize);
  }
  else
  {
    // Height follows the clipping viewport.
    const int contentHeight = mSearchArea->height();
    if (contentHeight <= 0)
      return;

    // Width remains at the complete expanded size.
    const int x = mDirection == Direction::Left ? std::min(0, mSearchArea->width() - mExpandedSize) : 0;

    mContentWidget->setGeometry(x, 0, mExpandedSize, contentHeight);
  }

  contentLayout->activate();
}