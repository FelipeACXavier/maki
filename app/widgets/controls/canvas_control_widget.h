#pragma once

#include <QColor>
#include <QGraphicsProxyWidget>
#include <functional>

#include "app_configs.h"

class QEnterEvent;
class QEvent;
class QHBoxLayout;
class QPropertyAnimation;
class QTimer;
class QWidget;

class CloseButton;
class StyledFrame;

namespace maki
{
class ControlWidget;
}

struct ControlProperties
{
  bool isFading = false;
  int fadeDurationMs = 150;
  int fadeDelayMs = 1000;

  QColor highlightColor;
  Config::ControlPosition position = Config::ControlPosition::Top;
};

class CanvasControlWidget : public QGraphicsProxyWidget
{
public:
  CanvasControlWidget(std::function<void()> dismissed, QGraphicsItem* parent = nullptr);

  maki::ControlWidget* controlWidget() const;
  void setControlWidget(maki::ControlWidget* controls, const ControlProperties& properties);

  bool hideControl();

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  const int FADE_DELAY_MS = 2000;
  const int FADE_DURATION_MS = 150;

  StyledFrame* mFrame = nullptr;
  QHBoxLayout* mFrameLayout = nullptr;
  maki::ControlWidget* mControls = nullptr;
  CloseButton* mCloseButton = nullptr;

  std::function<void()> mDismissed;

  ControlProperties mProperties;
  bool mMouseInside = false;
  QTimer* mFadeTimer = nullptr;
  QPropertyAnimation* mFadeAnimation = nullptr;

  void startFadeOut();
  void cancelFadeOut();
};