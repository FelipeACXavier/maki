#include "notification_widget.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include "app_configs.h"
#include "logging.h"
#include "style_helpers.h"

NotificationWidget::NotificationWidget(const QString& title, const QString& text, logging::LogLevel level, QWidget* parent)
    : StyledFrame(parent)
    , mOpacity(0.0)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  setBackgroundRole(StyledFrame::BackgroundRole::Base);
  setBorderRole(StyledFrame::BorderRole::Custom);
  setRadius(theme.borderRadius);
  setBorderWidth(theme.borderWidth);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto header = new StyledFrame(this);
  header->setBackgroundRole(StyledFrame::BackgroundRole::Midlight);
  header->setBorderRole(StyledFrame::BorderRole::None);
  header->setBorderWidth(2 * theme.borderWidth);

  // Header
  auto* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  headerLayout->setSpacing(5);

  auto* titleLabel = new oclero::qlementine::Label(title.size() > 1 ? title : "Notification", header);
  titleLabel->setRole(oclero::qlementine::TextRole::H3);
  titleLabel->setContentsMargins(0, 0, 0, 0);
  titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

  auto* statusBadge = new oclero::qlementine::StatusBadgeWidget(header);
  switch (level)
  {
    case logging::LogLevel::Warning:
      setCustomBorderColor(theme.statusColorWarning);
      statusBadge->setBadge(oclero::qlementine::StatusBadge::Warning);
      break;
    case logging::LogLevel::Error:
      setCustomBorderColor(theme.statusColorError);
      statusBadge->setBadge(oclero::qlementine::StatusBadge::Error);
      break;
    default:
      setCustomBorderColor(theme.statusColorSuccess);
      statusBadge->setBadge(oclero::qlementine::StatusBadge::Info);
      break;
  }
  statusBadge->setBadgeSize(oclero::qlementine::StatusBadgeSize::Medium);

  mCloseButton = new QPushButton(header);
  mCloseButton->setFlat(true);
  mCloseButton->setIcon(QIcon(":/icons/close.svg"));

  headerLayout->addWidget(statusBadge, 0, Qt::AlignVCenter);
  headerLayout->addSpacing(Config::CONTENT_PADDING);
  headerLayout->addWidget(titleLabel, 0, Qt::AlignVCenter);
  headerLayout->addStretch();
  headerLayout->addWidget(mCloseButton, 0, Qt::AlignVCenter);

  // Body
  auto body = new QWidget(this);
  body->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto* bodyLayout = new QVBoxLayout(body);
  bodyLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  bodyLayout->setAlignment(Qt::AlignVCenter);
  bodyLayout->setSpacing(0);

  auto* notificationText = new oclero::qlementine::Label(text, body);
  notificationText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  notificationText->setWordWrap(true);
  notificationText->setRole(oclero::qlementine::TextRole::Default);
  notificationText->setMinimumWidth(300 - 2 * Config::CONTENT_PADDING);
  notificationText->setMinimumHeight(2 * notificationText->fontMetrics().height());

  bodyLayout->addWidget(notificationText);

  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignLeft);

  layout->addWidget(header);
  layout->addWidget(body);

  // Opacity effect
  auto* effect = new QGraphicsOpacityEffect(this);
  setGraphicsEffect(effect);

  mFadeAnim = new QPropertyAnimation(this, "opacity", this);
  mFadeAnim->setDuration(200);
  mFadeAnim->setStartValue(opacity());
  mFadeAnim->setEndValue(1.0);

  connect(mCloseButton, &QPushButton::clicked, this, &NotificationWidget::hideAnimated);

  mAutoCloseTimer.setSingleShot(true);
  mAutoCloseTimer.setInterval(3000);

  connect(&mAutoCloseTimer, &QTimer::timeout, this, &NotificationWidget::hideAnimated);
}

qreal NotificationWidget::opacity() const
{
  return mOpacity;
}

void NotificationWidget::setOpacity(qreal o)
{
  mOpacity = std::clamp(o, 0.0, 1.0);
  if (auto* eff = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect()))
    eff->setOpacity(o);

  update();
}

void NotificationWidget::showAnimated()
{
  show();
  raise();
  mFadeAnim->setDirection(QAbstractAnimation::Forward);
  mFadeAnim->start();
  mAutoCloseTimer.start();
}

void NotificationWidget::hideAnimated()
{
  if (mFadeAnim->direction() == QAbstractAnimation::Backward && mFadeAnim->state() == QAbstractAnimation::Running)
    return;

  mAutoCloseTimer.stop();

  mFadeAnim->setStartValue(0.0);
  mFadeAnim->setEndValue(opacity());
  mFadeAnim->setDirection(QAbstractAnimation::Backward);

  connect(mFadeAnim, &QPropertyAnimation::finished, this, [this]() {
    emit dismissed(this);
    deleteLater();
  });

  mFadeAnim->start();
}
