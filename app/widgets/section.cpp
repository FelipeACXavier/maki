#include "section.h"

#include <QLabel>
#include <QPropertyAnimation>

#include "app_configs.h"
#include "logging.h"

constexpr double SPEED = 1.2;
constexpr int MIN_DURATION = 400;
constexpr int MAX_DURATION = 600;

SectionWidget::SectionWidget(QWidget* parent)
    : QWidget(parent)

{
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Wrap the actual content in an animatable container
  mContentArea = new CollapsibleArea(this);
  QVBoxLayout* contentLayout = new QVBoxLayout(mContentArea);
  contentLayout->setContentsMargins(0, 0, 0, 0);
  contentLayout->setSpacing(0);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  mContentArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void SectionWidget::addItem(QWidget* container, const QString& title)
{
  // Setup the content of the collapsable section
  mContent = container;
  mContent->setParent(mContentArea);
  mContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  mContentArea->layout()->addWidget(mContent);

  // Setup the header
  QWidget* header = new QWidget(this);
  header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QLabel* label = new QLabel(title, header);
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  label->setFont(Fonts::Label);
  label->setObjectName("SectionLabel");

  mToggleButton = new QToolButton(header);
  mToggleButton->setObjectName("SectionButton");
  mToggleButton->setCheckable(true);
  mToggleButton->setChecked(true);
  mToggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  mToggleButton->setArrowType(Qt::DownArrow);
  mToggleButton->setFixedWidth(30);
  mToggleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QHBoxLayout* hLayout = new QHBoxLayout(header);
  hLayout->setContentsMargins(2, 2, 10, 2);
  hLayout->setAlignment(Qt::AlignLeft);
  hLayout->setSpacing(0);
  hLayout->addWidget(mToggleButton);
  hLayout->addWidget(label);

  // Add the header and the content to the layout
  layout()->addWidget(header);
  layout()->addWidget(mContentArea);

  // With the header and content setup, we must define the animation
  header->setFixedHeight(header->sizeHint().height());
  mContentArea->setMaximumHeight(mContentArea->layout()->sizeHint().height());
  mContentArea->setMinimumHeight(0);

  auto* animation = new QPropertyAnimation(mContentArea, "contentHeight", this);
  animation->setEasingCurve(QEasingCurve::InOutQuad);

  connect(mToggleButton, &QToolButton::toggled, this, [this, animation](bool checked) {
    animation->stop();
    mToggleButton->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);

    if (checked)
    {
      // Always show before animating so there is actually something "appearing"
      mContentArea->show();
      const int targetHeight = mContentArea->maximumHeight();
      const int currentHeight = mContentArea->contentHeight();
      const int duration = getAnimationDuration(targetHeight, currentHeight);
      animation->setDuration(duration);
      animation->setStartValue(currentHeight);
      animation->setEndValue(targetHeight);
      LOG_TRACE("Section animation expand: %d to %d in %dms", currentHeight, targetHeight, duration);
    }
    else
    {
      const int targetHeight = 0;
      const int currentHeight = mContentArea->contentHeight();
      const int duration = getAnimationDuration(targetHeight, currentHeight);
      animation->setDuration(duration);
      animation->setStartValue(mContentArea->contentHeight());
      animation->setEndValue(0);
      LOG_TRACE("Section animation collapse: %d to %d in %dms", currentHeight, targetHeight, duration);

      connect(animation, &QPropertyAnimation::finished, mContentArea, [this]() {
        if (!mToggleButton->isChecked())
          mContentArea->hide(); }, Qt::SingleShotConnection);
    }

    animation->start();
  });
}

void SectionWidget::updateContentHeight(int height)
{
  if (!mContentArea)
    return;

  mContentArea->setContentHeight(height);
  mContentArea->setMaximumHeight(height);
  LOG_TRACE("New content area height: %d", height);
}

void SectionWidget::setExpanded(bool expanded)
{
  mToggleButton->setChecked(expanded);
}

bool SectionWidget::isExpanded() const
{
  return mToggleButton->isChecked();
}

QWidget* SectionWidget::content() const
{
  return mContent;
}

int SectionWidget::getAnimationDuration(int target, int current) const
{
  return std::clamp(static_cast<int>(std::abs(target - current) / SPEED), MIN_DURATION, MAX_DURATION);
}