#include "prompt.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

#include "base_dialog.h"
#include "frame.h"
#include "scroll_area.h"

namespace maki
{

PromptDialog::PromptDialog(const QString& title, const QString& accept, const QString& reject,
                           const QString& message, QWidget* parent)
    : BaseDialog(title, 4, 0.15, parent)
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  layout()->setSpacing(theme.spacing);
  layout()->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);

  // --- Top row: badge + text ---
  auto* header = new QWidget(this);
  header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  header->setObjectName("Header");

  auto* row = new QHBoxLayout(header);
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(theme.spacing);

  // Qlementine StatusBadge (example API)
  mBadge = new oclero::qlementine::StatusBadgeWidget(this);
  mBadge->setBadgeSize(oclero::qlementine::StatusBadgeSize::Medium);

  auto* label = new oclero::qlementine::Label(message, this);
  label->setRole(oclero::qlementine::TextRole::H5);
  label->setWordWrap(true);

  row->addWidget(mBadge);
  row->addWidget(label);

  layout()->addWidget(header);

  // --- Buttons ---
  auto* buttonBox = createButtons(accept, reject);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  limitWidth(buttonBox->sizeHint().width());
  setSize(4, 0.15);
}

void PromptDialog::setType(oclero::qlementine::StatusBadge type)
{
  mBadge->setBadge(type);
}

void PromptDialog::setExtraInfo(const QString& info)
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  auto* container = new StyledScrollArea(this);
  container->setBackgroundRole(StyledScrollArea::BackgroundRole::Window);
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  auto* frame = new StyledFrame(this);
  frame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  frame->setRadius(0);

  auto* frameLayout = new QVBoxLayout(frame);
  frameLayout->setSpacing(theme.spacing);
  frameLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);

  auto* label = new oclero::qlementine::Label(info, this);
  label->setRole(oclero::qlementine::TextRole::Default);
  label->setWordWrap(true);

  frameLayout->addWidget(label);
  container->setWidget(frame);

  // Make sure we have more space for the extra info
  auto* header = findChild<QWidget*>("Header");
  header->setMaximumHeight(size().height() / 4);

  layout()->insertWidget(layout()->count() - 1, container);

  // Update the size since there is some more content now
  setSize(2, 0.2);
}

bool confirmationPrompt(const QString& message, const QString& extraInfo, QWidget* parent)
{
  return confirmationPrompt(message, "Accept", "Reject", extraInfo, parent);
}

bool confirmationPrompt(const QString& message, const QString& confirm, const QString& reject, const QString& extraInfo, QWidget* parent)
{
  PromptDialog prompt("Confirmation", confirm, reject, message, parent);
  prompt.setType(oclero::qlementine::StatusBadge::Info);
  if (!extraInfo.isEmpty())
    prompt.setExtraInfo(extraInfo);

  return prompt.exec() == QDialog::Accepted;
}

bool warningPrompt(const QString& message, const QString& extraInfo, QWidget* parent)
{
  PromptDialog prompt("Warning", "Confirm", "Cancel", message, parent);
  prompt.setType(oclero::qlementine::StatusBadge::Warning);
  if (!extraInfo.isEmpty())
    prompt.setExtraInfo(extraInfo);

  return prompt.exec() == QDialog::Accepted;
}

bool errorPrompt(const QString& message, const QString& extraInfo, QWidget* parent)
{
  PromptDialog prompt("Error", "Confirm", "Cancel", message, parent);
  prompt.setType(oclero::qlementine::StatusBadge::Error);
  if (!extraInfo.isEmpty())
    prompt.setExtraInfo(extraInfo);

  return prompt.exec() == QDialog::Accepted;
}

}  // namespace maki
