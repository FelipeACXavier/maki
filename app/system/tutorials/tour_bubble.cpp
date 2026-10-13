#include "tour_bubble.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/Label.hpp>

TourBubble::TourBubble(QWidget* parent)
    : StyledFrame(parent, Qt::ToolTip)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  setFixedWidth(340);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(theme.spacing);

  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_NoSystemBackground, true);

  setBackgroundRole(StyledFrame::BackgroundRole::Base);
  setBorderRole(StyledFrame::BorderRole::Custom);
  setRadius(theme.borderRadius);
  setBorderWidth(2 * theme.borderWidth);
  setCustomBorderColor(theme.primaryColor);

  m_title = new oclero::qlementine::Label(this);
  m_title->setRole(oclero::qlementine::TextRole::H4);
  m_title->setWordWrap(true);
  m_title->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

  layout->addWidget(m_title);

  auto* line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  layout->addWidget(line);

  m_text = new oclero::qlementine::Label(this);
  m_text->setRole(oclero::qlementine::TextRole::Default);
  m_text->setWordWrap(true);
  m_text->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

  layout->addWidget(m_text);

  auto* buttons = new QHBoxLayout();
  m_stepLabel = new oclero::qlementine::Label(this);
  m_stepLabel->setRole(oclero::qlementine::TextRole::Default);

  buttons->addWidget(m_stepLabel);
  buttons->addStretch(1);

  m_back = new QPushButton(tr("Back"), this);
  m_back->setIcon(QIcon(":/icons/reset.svg"));
  buttons->addWidget(m_back);

  m_cancel = new QPushButton(tr("Cancel"), this);
  m_cancel->setIcon(QIcon(":/icons/circle-close.svg"));
  buttons->addWidget(m_cancel);

  m_next = new QPushButton(tr("Next"), this);
  m_next->setIcon(QIcon(":/icons/accept.svg"));
  m_next->setDefault(true);
  buttons->addWidget(m_next);

  layout->addLayout(buttons);
}

void TourBubble::setContent(const QString& title, const QString& text, int currentStep, int totalSteps, bool lastStep, bool showBack, bool showNext)
{
  m_title->setText(title);
  m_text->setText(text);

  m_stepLabel->setText(QStringLiteral("%1 / %2").arg(currentStep).arg(totalSteps));
  m_next->setText(lastStep ? tr("Finish") : tr("Next"));

  m_back->setVisible(showBack);
  m_next->setVisible(showNext);

  // Force Qt to recalculate word wrapping and vertical size hints.
  m_title->updateGeometry();
  m_text->updateGeometry();

  layout()->invalidate();
  layout()->activate();

  // Recalculate the required height at the fixed bubble width.
  const int requiredHeight = layout()->heightForWidth(width());

  if (requiredHeight >= 0)
    resize(width(), requiredHeight);
  else
    resize(width(), layout()->sizeHint().height());
}

QPushButton* TourBubble::nextButton() const
{
  return m_next;
}

QPushButton* TourBubble::backButton() const
{
  return m_back;
}

QPushButton* TourBubble::cancelButton() const
{
  return m_cancel;
}
