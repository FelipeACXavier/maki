#include "canvas_control_widget.h"

#include <QGraphicsOpacityEffect>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>

#include "app_configs.h"
#include "control_widget.h"
#include "logging.h"
#include "widgets/frame.h"

CanvasControlWidget::CanvasControlWidget(std::function<void()> dismissed, QGraphicsItem* parent)
    : QGraphicsProxyWidget(parent)
    , mDismissed(std::move(dismissed))
{
  mFrame = new StyledFrame();

  mFrame->setAttribute(Qt::WA_TranslucentBackground);
  mFrame->setAutoFillBackground(false);

  mFrame->setBackgroundRole(StyledFrame::BackgroundRole::Midlight);
  mFrame->setBorderRole(StyledFrame::BorderRole::Highlight);

  mFrameLayout = new QHBoxLayout(mFrame);

  if (const auto* qlementineStyle = oclero::qlementine::appStyle())
  {
    const auto theme = qlementineStyle->theme();

    mFrame->setRadius(theme.borderRadius);
    mFrame->setBorderWidth(theme.borderWidth);

    mFrameLayout->setContentsMargins(0, 0, 0, 0);
    mFrameLayout->setSpacing(0);
  }

  QGraphicsProxyWidget::setWidget(mFrame);

  setZValue(1000);

  // --------------------------------------------------------------------------
  // Fade timer
  mFadeTimer = new QTimer(this);
  mFadeTimer->setSingleShot(true);
  mFadeTimer->setInterval(FADE_DELAY_MS);

  connect(mFadeTimer, &QTimer::timeout, this, &CanvasControlWidget::startFadeOut);

  // --------------------------------------------------------------------------
  // Fade animation
  auto* opacityEffect = new QGraphicsOpacityEffect(this);
  opacityEffect->setOpacity(1.0);
  setGraphicsEffect(opacityEffect);

  mFadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
  connect(mFadeAnimation, &QPropertyAnimation::finished, this, [this, opacityEffect]() {
    if (opacityEffect->opacity() > 0.0)
      return;

    if (mDismissed)
      mDismissed();

    opacityEffect->setOpacity(1.0);
  });
}

maki::ControlWidget* CanvasControlWidget::controlWidget() const
{
  return mControls;
}

void CanvasControlWidget::setControlWidget(maki::ControlWidget* controls, const ControlProperties& properties)
{
  if (!controls)
    return;

  mProperties = properties;

  if (mControls)
  {
    mFrameLayout->removeWidget(mControls);
    mControls->removeEventFilter(this);
    mControls->setParent(nullptr);
  }

  if (mFadeTimer)
    mFadeTimer->setInterval(mProperties.fadeDelayMs);

  mControls = controls;
  mControls->setParent(mFrame);
  mControls->installEventFilter(this);

  mFrameLayout->insertWidget(0, mControls);

  // Force the QWidget layout to calculate its contents.
  mFrameLayout->invalidate();
  mFrameLayout->activate();

  const QSize size = mFrameLayout->sizeHint();
  mFrame->setFixedSize(size);

  // Synchronize QGraphicsProxyWidget geometry explicitly.
  resize(size);

  connect(controls, &maki::ControlWidget::dismissed, this, [this] {
    cancelFadeOut();

    if (mDismissed)
      mDismissed();
  });

  if (mFadeTimer)
    mFadeTimer->start();
}

bool CanvasControlWidget::eventFilter(QObject* watched, QEvent* event)
{
  switch (event->type())
  {
    case QEvent::UpdateRequest:
    case QEvent::Paint:
    case QEvent::LayoutRequest:
      update();
      if (scene())
        scene()->update(sceneBoundingRect());
      break;
    default:
      break;
  }

  return QGraphicsProxyWidget::eventFilter(watched, event);
}

void CanvasControlWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  mMouseInside = true;
  cancelFadeOut();
  QGraphicsProxyWidget::hoverEnterEvent(event);
}

void CanvasControlWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  mMouseInside = false;

  // Don't disappear immediately when the user accidentally moves a few pixels away from the popup.
  if (mFadeTimer)
    mFadeTimer->start();

  QGraphicsProxyWidget::hoverLeaveEvent(event);
}

void CanvasControlWidget::startFadeOut()
{
  if (!mProperties.isFading || mMouseInside)
    return;

  auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect());
  if (!effect)
  {
    if (mDismissed)
      mDismissed();
  }

  mFadeAnimation->stop();
  mFadeAnimation->setDuration(mProperties.fadeDurationMs);
  mFadeAnimation->setStartValue(effect->opacity());
  mFadeAnimation->setEndValue(0.0);
  mFadeAnimation->start();
}

void CanvasControlWidget::cancelFadeOut()
{
  if (!mProperties.isFading)
    return;

  if (mFadeTimer)
    mFadeTimer->stop();

  if (mFadeAnimation)
    mFadeAnimation->stop();

  if (auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect()))
    effect->setOpacity(1.0);
}

bool CanvasControlWidget::hideControl()
{
  startFadeOut();

  return mProperties.isFading;
}