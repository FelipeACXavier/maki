#include "notification_widget.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "logging.h"
#include "style_helpers.h"
#include "theme.h"

NotificationWidget::NotificationWidget(const QString& title, const QString& text, logging::LogLevel level, QWidget* parent)
    : QFrame(parent)
    , m_opacity(0.0)
{
  setObjectName("NotificationToast");
  setFrameShape(QFrame::NoFrame);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto header = new QWidget(this);
  header->setObjectName("NotificationHeader");

  // Use dynamic property for theming based on level
  QString iconPath = "";
  switch (level)
  {
    case logging::LogLevel::Warning:
      iconPath = ":/icons/warning.svg";
      setProperty("level", "warning");
      header->setProperty("level", "warning");
      break;
    case logging::LogLevel::Error:
      iconPath = ":/icons/error.svg";
      setProperty("level", "error");
      header->setProperty("level", "error");
      break;
    case logging::LogLevel::Trace:
    case logging::LogLevel::Debugging:
    case logging::LogLevel::Info:
    default:
      iconPath = ":/icons/info.svg";
      setProperty("level", "info");
      header->setProperty("level", "info");
      break;
  }

  // Header
  auto* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(6, 0, 6, 0);
  headerLayout->setSpacing(0);

  QLabel* titleLabel = new QLabel(title.size() > 1 ? title : "Notification", this);
  titleLabel->setObjectName("NotificationTitle");
  titleLabel->setContentsMargins(0, 0, 0, 0);
  titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

  QLabel* titleIcon = new QLabel();
  titleIcon->setPixmap(applyColorToIcon(iconPath, Config::FOREGROUND).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  mCloseButton = new QPushButton(this);
  mCloseButton->setObjectName("FlatButton");
  mCloseButton->setFlat(true);
  mCloseButton->setIcon(addIconWithColor(":/icons/close.svg", Config::FOREGROUND));

  headerLayout->addWidget(titleIcon, 0, Qt::AlignVCenter);
  headerLayout->addSpacing(6);
  headerLayout->addWidget(titleLabel, 0, Qt::AlignVCenter);
  headerLayout->addStretch();
  headerLayout->addWidget(mCloseButton, 0, Qt::AlignVCenter);

  // Body
  auto body = new QWidget(this);
  body->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  body->setObjectName("NotificationBody");

  auto* bodyLayout = new QHBoxLayout(body);
  bodyLayout->setContentsMargins(6, 6, 6, 6);
  bodyLayout->setSpacing(0);

  mLabel = new QLabel(text, this);
  mLabel->setWordWrap(true);

  bodyLayout->addWidget(mLabel);

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
  mAutoCloseTimer.setInterval(4000);

  connect(&mAutoCloseTimer, &QTimer::timeout, this, &NotificationWidget::hideAnimated);
}

qreal NotificationWidget::opacity() const
{
  return m_opacity;
}

void NotificationWidget::setOpacity(qreal o)
{
  m_opacity = o;
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
