#include "long_notification.h"

#include <QVBoxLayout>

#include "style_helpers.h"

namespace maki
{
LongNotificationWidget::LongNotificationWidget(const QString& id, const QString& title, logging::LogLevel level, QWidget* parent)
    : NotificationWidget(title, "", level, parent)
    , mId(id)
{
}

QString LongNotificationWidget::id() const
{
  return mId;
}

void LongNotificationWidget::updateContent(QWidget* content)
{
  if (content == nullptr)
  {
    setupAlarm(3000);
    mFadeAnim->setDuration(duration() * 2);
    mAutoCloseTimer.start();
    return;
  }

  clearLayout(mBody->getContent());

  content->setParent(this);
  mBody->addCollapsableWidget(content);

  // Adjust to content size
  mBody->setExpandedSize(content->sizeHint().height());
  LOG_DEBUG("Setting expanded height: %d", content->sizeHint().height());
}

bool LongNotificationWidget::disappearing() const
{
  return false;
}

}  // namespace maki