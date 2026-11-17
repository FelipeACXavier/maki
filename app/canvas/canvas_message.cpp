#include "canvas_message.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

#include "app_configs.h"
#include "widgets/close_button.h"
#include "widgets/frame.h"

CanvasMessage::CanvasMessage(const QString& message, std::function<void()> dismissed, QGraphicsItem* parent)
    : QGraphicsProxyWidget(parent)
    , mDismissed(std::move(dismissed))
{
  auto* frame = new StyledFrame();

  // Important: don't let QWidget paint a rectangular background
  frame->setAttribute(Qt::WA_TranslucentBackground);
  frame->setAutoFillBackground(false);

  frame->setBackgroundRole(StyledFrame::BackgroundRole::Midlight);
  frame->setBorderRole(StyledFrame::BorderRole::Highlight);

  auto* layout = new QHBoxLayout(frame);
  if (const auto* qlementineStyle = oclero::qlementine::appStyle())
  {
    const auto theme = qlementineStyle->theme();
    frame->setRadius(theme.borderRadius);
    frame->setBorderWidth(theme.borderWidth);
    layout->setContentsMargins(theme.spacing, theme.spacing / 2, theme.spacing / 2, theme.spacing / 2);
    layout->setSpacing(theme.spacing);
  }

  auto* label = new QLabel(message, frame);
  label->setObjectName("messageLabel");
  label->setWordWrap(true);
  label->setMaximumWidth(500);

  auto* closeButton = new CloseButton(frame);
  closeButton->setFixedSize(Config::SMALL_BUTTON_SIZE);

  layout->addWidget(label);
  layout->addWidget(closeButton, 0, Qt::AlignTop);

  setWidget(frame);
  setZValue(1000);

  QObject::connect(closeButton, &CloseButton::clicked, this, [this]() {
    if (mDismissed)
      mDismissed();
  });
}

void CanvasMessage::setMessage(const QString& message)
{
  if (!widget())
    return;

  if (auto* closeButton = findChild<CloseButton*>())
  {
    closeButton->setDown(false);
    closeButton->update();
  }

  if (auto* label = widget()->findChild<QLabel*>("messageLabel"))
    label->setText(message);
}