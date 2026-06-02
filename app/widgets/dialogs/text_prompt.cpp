#include "text_prompt.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

namespace maki
{
TextDialog::TextDialog(const QString& title, const QString& prompt, QWidget* parent)
    : BaseDialog(title, 4, 0.15, parent)
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  layout()->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  layout()->setSpacing(theme.spacing);

  // --- Top row: badge + text ---
  auto* header = new QWidget(this);
  header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  auto* row = new QHBoxLayout(header);
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(theme.spacing);

  auto badge = new oclero::qlementine::StatusBadgeWidget(this);
  badge->setBadge(oclero::qlementine::StatusBadge::Info);
  badge->setBadgeSize(oclero::qlementine::StatusBadgeSize::Medium);

  auto* label = new oclero::qlementine::Label(prompt, this);
  label->setRole(oclero::qlementine::TextRole::H5);
  label->setWordWrap(true);

  row->addWidget(badge);
  row->addWidget(label);

  layout()->addWidget(header);

  // --- Input ---
  mNameEdit = new QLineEdit(this);
  mNameEdit->setClearButtonEnabled(true);

  layout()->addWidget(mNameEdit);

  // --- Buttons ---
  auto* buttonBox = createButtons(tr("Accept"), tr("Cancel"));
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  limitWidth(buttonBox->sizeHint().width());
  setSize(3, 0.1);

  mNameEdit->setFocus();
}

QString TextDialog::Value() const
{
  return mNameEdit->text();
}

QString textPrompt(const QString& title, const QString& name, QWidget* parent)
{
  TextDialog prompt(title, name, parent);

  if (prompt.exec() != QDialog::Accepted)
    return QString();

  return prompt.Value().trimmed();
}

}  // namespace maki
