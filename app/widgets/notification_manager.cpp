#include "notification_manager.h"

#include <QGuiApplication>
#include <QScreen>

#include "theme.h"

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

void NotificationManager::onToastDismissed(NotificationWidget* toast)
{
  mToasts.removeAll(toast);
  repositionToasts();
}

void NotificationManager::repositionToasts()
{
  if (!mParentWindow)
    return;

  const auto leftMargin = Config::getValueFromTheme("@notification_left_margin");
  if (!leftMargin.isValid())
    return;
  const auto topMargin = Config::getValueFromTheme("@notification_top_margin");
  if (!topMargin.isValid())
    return;
  const auto betweenMargin = Config::getValueFromTheme("@notification_between_margin");
  if (!betweenMargin.isValid())
    return;

  int y = topMargin.toInt();

  // Place relative to the parent window
  QRect pw = mParentWindow->rect();

  for (auto* toast : mToasts)
  {
    if (!toast)
      continue;

    // For some reason, sizeHint must be called before so width() and height() are correct
    toast->resize(toast->sizeHint());
    int x = pw.width() - toast->width() - leftMargin.toInt();
    toast->move(x, y);
    y += toast->height() + betweenMargin.toInt();
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