
// notificationwidget.h
#pragma once

#include <QFrame>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>

#include "logging.h"

class NotificationWidget : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
  NotificationWidget(const QString& title, const QString& text, logging::LogLevel level, QWidget* parent = nullptr);

  qreal opacity() const;
  void setOpacity(qreal o);

signals:
  void dismissed(NotificationWidget* self);

public slots:
  void showAnimated();
  void hideAnimated();

private:
  QLabel* mLabel = nullptr;
  QPushButton* mCloseButton = nullptr;
  QPropertyAnimation* mFadeAnim = nullptr;
  QTimer mAutoCloseTimer;
  qreal m_opacity = 1.0;
};
