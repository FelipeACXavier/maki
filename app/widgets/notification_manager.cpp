#include "notification_manager.h"

#include <QGuiApplication>
#include <QScreen>

#include "theme.h"

NotificationManager::NotificationManager(QWidget* parentWindow, QObject* parent)
    : QObject(parent)
    , mParentWindow(parentWindow)
{
}

void NotificationManager::showNotification(const QString& text, logging::LogLevel level)
{
  if (!mParentWindow)
    return;

  auto* toast = new NotificationWidget(text, level, mParentWindow);
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

  const auto margin = Config::getValueFromTheme("@notification_margin");
  if (!margin.isValid())
    return;

  int y = margin.toInt();

  // Place relative to the parent window
  QRect pw = mParentWindow->rect();

  for (auto* toast : mToasts)
  {
    if (!toast)
      continue;

    // For some reason, sizeHint must be called before so width() and height() are correct
    toast->sizeHint();
    int x = pw.right() - toast->width() - margin.toInt();
    toast->move(x, y);
    y += toast->height() + margin.toInt();
  }
}
