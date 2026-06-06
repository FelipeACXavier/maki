#include "notification_manager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QUuid>

#include "app_configs.h"
#include "long_notification.h"

NotificationManager::NotificationManager(QWidget* parentWindow, QObject* parent)
    : QObject(parent)
    , mParentWindow(parentWindow)
{
  // This lets us listen to resize events of the parent so the notification toasts can react to layout changes.
  mParentWindow->installEventFilter(this);
}

void NotificationManager::showNotification(const QString& header, const QString& text, logging::LogLevel level)
{
  if (!mParentWindow)
    return;

  auto* toast = new NotificationWidget(header, text, level, mParentWindow);
  toast->hide();

  connect(toast, &NotificationWidget::dismissed, this, &NotificationManager::onToastDismissed);

  mToasts.prepend(toast);
  repositionToasts();

  toast->showAnimated();
}

QString NotificationManager::showLongNotification(const QString& id, const QString& header, QWidget* contents, logging::LogLevel level)
{
  if (!mParentWindow)
    return QString();

  auto uuid = updateExistingNotification(id, contents);
  if (!uuid.isEmpty())
    return uuid;

  uuid = QUuid::createUuid().toString();
  auto* toast = new maki::LongNotificationWidget(uuid, header, level, mParentWindow);
  toast->updateContent(contents);
  toast->hide();

  connect(toast, &NotificationWidget::dismissed, this, &NotificationManager::onToastDismissed);

  mToasts.prepend(toast);
  repositionToasts();

  toast->showAnimated();

  return uuid;
}

QString NotificationManager::updateExistingNotification(const QString& id, QWidget* contents)
{
  if (id.isEmpty())
    return QString();

  for (const auto& t : mToasts)
  {
    if (t->disappearing())
      continue;

    if (auto* d = qobject_cast<maki::LongNotificationWidget*>(t))
    {
      if (d->id() != id)
        continue;

      d->updateContent(contents);
      d->showAnimated();
      return id;
    }
  }

  return QString();
}

void NotificationManager::onToastDismissed(NotificationWidget* toast)
{
  mToasts.removeAll(toast);
  repositionToasts();
}

void NotificationManager::repositionToasts()
{
  if (!mParentWindow)
    return;

  int y = Config::CANVAS_OVERLAY_Y_MARGIN;
  const auto leftMargin = Config::CANVAS_OVERLAY_X_MARGIN;
  const auto betweenMargin = Config::CANVAS_OVERLAY_X_MARGIN;

  // Place relative to the parent window
  const QRect pw = mParentWindow->rect();

  auto reposition = [&](NotificationWidget* toast, int y) {
    // For some reason, sizeHint must be called before so width() and height() are correct
    toast->resize(toast->sizeHint());
    int x = pw.width() - toast->width() - leftMargin;
    toast->move(x, y);
    return toast->height() + betweenMargin;
  };

  // First the long running toasts
  for (auto* toast : mToasts)
  {
    if (!toast || toast->disappearing())
      continue;

    y += reposition(toast, y);
  }

  // Then the dissappearing ones
  for (auto* toast : mToasts)
  {
    if (!toast || !toast->disappearing())
      continue;

    y += reposition(toast, y);
  }
}

bool NotificationManager::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == mParentWindow)
  {
    switch (event->type())
    {
      case QEvent::Resize:
      case QEvent::Move:
      case QEvent::Show:
      case QEvent::Hide:
      case QEvent::LayoutRequest:
        repositionToasts();
        break;
      default:
        break;
    }
  }

  return QObject::eventFilter(watched, event);
}
