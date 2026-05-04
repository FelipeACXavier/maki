#include "notification_widget.h"

#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include "app_configs.h"
#include "logging.h"
#include "style_helpers.h"

NotificationWidget::NotificationWidget(const QString& title, const QString& text, logging::LogLevel level, QWidget* parent)
    : StyledFrame(parent)
    , mAlarmSetup(false)
    , mFadeAnim(nullptr)
    , mCloseButton(nullptr)
    , mOpacity(0.0)
    , mContentLayout(nullptr)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  setBackgroundRole(StyledFrame::BackgroundRole::Base);
  setBorderRole(StyledFrame::BorderRole::Custom);
  setRadius(theme.borderRadius);
  setBorderWidth(theme.borderWidth);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  // Make sure the notifications don't take too much space
  QScreen* screen = this->screen();
  if (!screen)
    screen = QGuiApplication::primaryScreen();

  setFixedWidth(screen->availableGeometry().width() / 7);

  auto header = new StyledFrame(this);
  header->setBackgroundRole(StyledFrame::BackgroundRole::Midlight);
  header->setBorderRole(StyledFrame::BorderRole::None);
  header->setBorderWidth(2 * theme.borderWidth);
  header->setRadius(theme.borderRadius);

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
  body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  mContentLayout = new QVBoxLayout(body);
  mContentLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  mContentLayout->setAlignment(Qt::AlignVCenter);
  mContentLayout->setSpacing(0);

  if (!text.isEmpty())
  {
    auto* notificationText = new oclero::qlementine::Label(text, body);
    notificationText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    notificationText->setWordWrap(true);
    notificationText->setRole(oclero::qlementine::TextRole::Default);
    notificationText->setMinimumWidth(300 - 2 * Config::CONTENT_PADDING);
    notificationText->setMinimumHeight(2 * notificationText->fontMetrics().height());

    mContentLayout->addWidget(notificationText);
  }

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
  mFadeAnim->setDuration(duration());
  mFadeAnim->setStartValue(opacity());
  mFadeAnim->setEndValue(1.0);

  connect(mCloseButton, &QPushButton::clicked, this, &NotificationWidget::hideAnimated);

  // Only enable in short notification
  if (!text.isEmpty())
    setupAlarm(3000);
}

int NotificationWidget::duration() const
{
  return 350;
}

void NotificationWidget::setupAlarm(int msec)
{
  mAlarmSetup = true;
  mAutoCloseTimer.setSingleShot(true);
  mAutoCloseTimer.setInterval(msec);
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

bool NotificationWidget::disappearing() const
{
  return true;
}

QVBoxLayout* NotificationWidget::getContent()
{
  return mContentLayout;
}

void NotificationWidget::showAnimated()
{
  if (opacity() == 1.0)
    return;

  show();
  raise();
  mFadeAnim->setDirection(QAbstractAnimation::Forward);
  mFadeAnim->start();
  if (mAlarmSetup)
    mAutoCloseTimer.start();
}

void NotificationWidget::hideAnimated()
{
  if (mFadeAnim->direction() == QAbstractAnimation::Backward && mFadeAnim->state() == QAbstractAnimation::Running)
    return;

  if (mAlarmSetup)
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
