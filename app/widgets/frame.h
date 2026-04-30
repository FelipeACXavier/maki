#pragma once

#include <QFrame>
#include <QPainter>
#include <oclero/qlementine.hpp>

#pragma once

#include <QColor>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>

class StyledFrame : public QFrame
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

  enum class BorderRole
  {
    None,
    Mid,
    Highlight,
    Custom
  };

  StyledFrame(QWidget* parent = nullptr);

  void setBackgroundRole(BackgroundRole role);
  void setBorderRole(BorderRole role);
  void setCustomBackgroundColor(const QColor& color);
  void setCustomBorderColor(const QColor& color);
  void setBorderWidth(int width);
  void setRadius(qreal radius);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  BackgroundRole mBackgroundRole = BackgroundRole::Base;
  BorderRole mBorderRole = BorderRole::Mid;

  QColor mCustomBackgroundColor;
  QColor mCustomBorderColor;

  int mBorderWidth = 1;
  qreal mRadius = 0.0;

  QColor backgroundColor() const;
  QColor borderColor() const;
};