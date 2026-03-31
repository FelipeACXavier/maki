#pragma once

#include <QList>
#include <QObject>
#include <QPointer>

#include "logging.h"
#include "widgets/notification_widget.h"

class NotificationManager : public QObject
{
  Q_OBJECT
public:
  explicit NotificationManager(QWidget* parentWindow, QObject* parent = nullptr);

public slots:
  void showNotification(const QString& header, const QString& text, logging::LogLevel level);

private slots:
  void onToastDismissed(NotificationWidget* toast);

private:
  void repositionToasts();
  bool eventFilter(QObject* watched, QEvent* event);

  QPointer<QWidget> mParentWindow;
  QList<NotificationWidget*> mToasts;
};
