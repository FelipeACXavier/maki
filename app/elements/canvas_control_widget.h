#pragma once

#include <QGraphicsProxyWidget>
#include <functional>

class QHBoxLayout;
class QWidget;
class CloseButton;
class StyledFrame;

class CanvasControlWidget : public QGraphicsProxyWidget
{
public:
  explicit CanvasControlWidget(QWidget* controls, std::function<void()> dismissed, QGraphicsItem* parent = nullptr);

  void setControlWidget(QWidget* controls);

private:
  StyledFrame* mFrame = nullptr;
  QHBoxLayout* mFrameLayout = nullptr;
  QWidget* mControls = nullptr;
  CloseButton* mCloseButton = nullptr;

  std::function<void()> mDismissed;
};