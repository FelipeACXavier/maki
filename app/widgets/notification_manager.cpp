#include "notification_manager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QUuid>

#include "app_configs.h"
#include "long_notification.h"
#include "style_helpers.h"

NotificationManager::NotificationManager(QWidget* parentWindow, QObject* parent)
    : QObject(parent)
    , mParentWindow(parentWindow)
    , mMinimize(false)
{
  // This lets us listen to resize events of the parent so the notification toasts can react to layout changes.
  mParentWindow->installEventFilter(this);
}

void NotificationManager::toggleMinimize(bool minimize)
{
  // Expand or minimize all the toasts
  for (const auto& t : mToasts)
    t->minimize(minimize);

  mMinimize = minimize;
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

  // Oh, the classic layout timing issue. Right after constructing the toast and inserting some content, Qt has not
  // yet completed the layout pass that determines the toast’s final width or the wrapped/content-dependent height,
  // We must, thus, defer the initial minimize/expand call until after Qt has processed the initial layout.
  QMetaObject::invokeMethod(toast, [toast, minimize = mMinimize]() {
        if (toast)
          toast->minimize(minimize); }, Qt::QueuedConnection);

  toast->showAnimated();
}

QString NotificationManager::showLongNotification(const QString& id, const QString& header, QWidget* contents, logging::LogLevel level)
{
  if (!mParentWindow)
    return QString();

  auto uuid = updateExistingNotification(id, contents, level);
  if (!uuid.isEmpty())
    return uuid;

  uuid = QUuid::createUuid().toString();
  auto* toast = new maki::LongNotificationWidget(uuid, header, level, mParentWindow);
  toast->updateContent(contents);
  toast->hide();

  connect(toast, &NotificationWidget::dismissed, this, &NotificationManager::onToastDismissed);

  mToasts.prepend(toast);
  repositionToasts();

  // See showNotification above
  QMetaObject::invokeMethod(toast, [toast, minimize = mMinimize]() {
        if (toast)
          toast->minimize(minimize); }, Qt::QueuedConnection);

  toast->showAnimated();

  return uuid;
}

QString NotificationManager::updateExistingNotification(const QString& id, QWidget* contents, logging::LogLevel level)
{
  if (id.isEmpty())
    return QString();

  for (const auto& t : mToasts)
  {
    if (t->disappearing())
      continue;

    if (auto* widget = qobject_cast<maki::LongNotificationWidget*>(t); widget)
    {
      if (widget->id() != id)
        continue;

      widget->setBadge(logLevelToStatusBadge(level));
      widget->updateContent(contents);
      widget->showAnimated();
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
