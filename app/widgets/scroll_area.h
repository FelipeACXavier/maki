#pragma once

#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>

class StyledScrollArea : public QScrollArea
{
  Q_OBJECT

public:
  enum class BackgroundRole
  {
    Window,
    Base,
    Button,
    Midlight,
    Custom
  };

  StyledScrollArea(QWidget* parent = nullptr);

  void setBackgroundRole(BackgroundRole role);
  void setCustomBackgroundColor(const QColor& color);

protected:
  bool viewportEvent(QEvent* event) override;

private:
  BackgroundRole mBackgroundRole = BackgroundRole::Base;
  QColor mCustomBackgroundColor;

  QColor backgroundColor() const;
};