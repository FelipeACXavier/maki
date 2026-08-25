#include "canvas_control_widget.h"

#include <QHBoxLayout>

#include "app_configs.h"
#include "widgets/close_button.h"
#include "widgets/frame.h"

CanvasControlWidget::CanvasControlWidget(QWidget* controls, std::function<void()> dismissed, QGraphicsItem* parent)
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

  // mCloseButton = new CloseButton(mFrame);
  // mCloseButton->setFixedSize(Config::SMALL_BUTTON_SIZE);

  // mFrameLayout->addWidget(mCloseButton, 0, Qt::AlignTop);

  // connect(mCloseButton, &CloseButton::clicked, this, [this]() {
  //   if (mDismissed)
  //     mDismissed();
  // });

  QGraphicsProxyWidget::setWidget(mFrame);

  setZValue(1000);
  setControlWidget(controls);
}

void CanvasControlWidget::setControlWidget(QWidget* controls)
{
  if (!controls)
    return;

  if (mControls)
  {
    mFrameLayout->removeWidget(mControls);
    mControls->setParent(nullptr);
  }

  mControls = controls;
  mControls->setParent(mFrame);

  mFrameLayout->insertWidget(0, mControls);

  // Force the QWidget layout to calculate its contents.
  mFrameLayout->invalidate();
  mFrameLayout->activate();

  const QSize size = mFrameLayout->sizeHint();
  mFrame->setFixedSize(size);

  // Synchronize QGraphicsProxyWidget geometry explicitly.
  resize(size);
}