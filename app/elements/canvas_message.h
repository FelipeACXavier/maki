#pragma once

#include <QGraphicsProxyWidget>
#include <QString>
#include <functional>

class CanvasMessage : public QGraphicsProxyWidget
{
public:
  CanvasMessage(const QString& message, std::function<void()> dismissed = {}, QGraphicsItem* parent = nullptr);

  void setMessage(const QString& message);

private:
  std::function<void()> mDismissed;
};