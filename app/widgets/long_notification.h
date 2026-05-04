#pragma once

#include "notification_widget.h"

namespace maki
{
class LongNotificationWidget : public NotificationWidget
{
  Q_OBJECT
public:
  LongNotificationWidget(const QString& id, const QString& title, logging::LogLevel level, QWidget* parent = nullptr);

  void updateContent(QWidget* content);

  QString id() const;

  bool disappearing() const override;

private:
  const QString mId;
};
}  // namespace maki