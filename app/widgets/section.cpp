#include "section.h"

#include <QLabel>
#include <QPropertyAnimation>
#include <oclero/qlementine/widgets/Label.hpp>

#include "app_configs.h"
#include "clickable_icon.h"
#include "logging.h"

constexpr double SPEED = 1.2;
constexpr int MIN_DURATION = 400;
constexpr int MAX_DURATION = 600;

SectionWidget::SectionWidget(QWidget* parent)
{
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Wrap the actual content in an animatable container
  mContentArea = new CollapsibleAreaHeight(this);
  QVBoxLayout* contentLayout = new QVBoxLayout(mContentArea);
  contentLayout->setContentsMargins(0, 0, 0, 0);
  contentLayout->setSpacing(0);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  mContentArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void SectionWidget::addItem(QWidget* container, const QString& title)
{
  addItem(container, title, oclero::qlementine::TextRole::H4);
}

void SectionWidget::addItem(QWidget* container, const QString& title, oclero::qlementine::TextRole role)
{
  // Setup the content of the collapsable section
  mContent = container;
  mContent->setParent(mContentArea);
  mContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  mContentArea->layout()->addWidget(mContent);

  // Setup the header
  QWidget* header = new QWidget(this);
  header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto* label = new oclero::qlementine::Label(title, header);
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  label->setRole(role);
  label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

  auto* line = new QFrame(header);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Plain);

  auto* lableLayout = new QVBoxLayout();
  lableLayout->addWidget(label);
  lableLayout->addWidget(line);

  mToggleButton = new ClickableIcon(QIcon(":/icons/arrow-down.svg"), QSize(16, 16), header);
  mToggleButton->setCheckable(true);
  mToggleButton->setChecked(true);
  mToggleButton->setFixedWidth(30);
  mToggleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QHBoxLayout* hLayout = new QHBoxLayout(header);
  hLayout->setContentsMargins(2, 2, 10, 2);
  hLayout->setAlignment(Qt::AlignLeft);
  hLayout->setSpacing(4);
  hLayout->addWidget(mToggleButton);
  hLayout->addLayout(lableLayout);

  // Add the header and the content to the layout
  layout()->addWidget(header);
  layout()->addWidget(mContentArea);

  // With the header and content setup, we must define the mAnimation
  header->setFixedHeight(header->sizeHint().height());
  mContentArea->setMaximumHeight(mContentArea->layout()->sizeHint().height());
  mContentArea->setMinimumHeight(0);

  mAnimation = new QPropertyAnimation(mContentArea, "contentHeight", this);
  mAnimation->setEasingCurve(QEasingCurve::InOutQuad);

  connect(mToggleButton, &ClickableIcon::toggled, this, &SectionWidget::toggled);
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
  mToggleButton->setIcon(expanded ? QIcon(":/icons/arrow-down.svg") : QIcon(":/icons/arrow-right.svg"));
  mToggleButton->setChecked(expanded);
}

bool SectionWidget::isExpanded() const
{
  return mToggleButton->isChecked();
}

void SectionWidget::setDuration(int duration)
{
  if (duration < 0)
    mDuration = std::nullopt;
  else
    mDuration = duration;
}

void SectionWidget::toggled(bool checked)
{
  mAnimation->stop();
  mToggleButton->setIcon(checked ? QIcon(":/icons/arrow-down.svg") : QIcon(":/icons/arrow-right.svg"));

  if (checked)
  {
    // Always show before animating so there is actually something "appearing"
    mContentArea->show();
    const int targetHeight = mContentArea->maximumHeight();
    const int currentHeight = mContentArea->contentHeight();
    const int duration = getAnimationDuration(targetHeight, currentHeight);
    mAnimation->setDuration(duration);
    mAnimation->setStartValue(currentHeight);
    mAnimation->setEndValue(targetHeight);
    LOG_TRACE("Section mAnimation expand: %d to %d in %dms", currentHeight, targetHeight, duration);
  }
  else
  {
    const int targetHeight = 0;
    const int currentHeight = mContentArea->contentHeight();
    const int duration = getAnimationDuration(targetHeight, currentHeight);
    mAnimation->setDuration(duration);
    mAnimation->setStartValue(mContentArea->contentHeight());
    mAnimation->setEndValue(0);
    LOG_TRACE("Section mAnimation collapse: %d to %d in %dms", currentHeight, targetHeight, duration);

    connect(mAnimation, &QPropertyAnimation::finished, mContentArea, [this]() {
      if (!mToggleButton->isChecked())
        mContentArea->hide(); }, Qt::SingleShotConnection);
  }

  mAnimation->start();
}

QWidget* SectionWidget::content() const
{
  return mContent;
}

int SectionWidget::getAnimationDuration(int target, int current) const
{
  return mDuration ? mDuration.value() : std::clamp(static_cast<int>(std::abs(target - current) / SPEED), MIN_DURATION, MAX_DURATION);
}