#include "type_selector.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QScrollBar>
#include <QToolButton>

#include "../clickable_icon.h"
#include "../dialogs/type_reference_dialog.h"
#include "../widget_factory.h"
#include "app_configs.h"
#include "type_registry.h"

namespace maki
{
TypeSelector::TypeSelector(const QString& objectName, QWidget* parent)
    : QWidget(parent)
    , mUsesPrimitives(false)
    , mTitle(tr("Select type"))
{
  setObjectName(objectName);

  auto* containerLayout = new QHBoxLayout(this);
  containerLayout->setContentsMargins(0, 0, 0, 0);
  containerLayout->setAlignment(Qt::AlignTop);

  mSelector = createComboBox();
  populateTypes();

  auto* targetButton = new ClickableIcon(QIcon(":/icons/bars.svg"), Config::SMALL_BUTTON_SIZE, this);
  targetButton->setToolTip(tr("More types"));
  targetButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);

  connect(targetButton, &ClickableIcon::clicked, this, [this] {
    maki::TypeReferenceDialog dialog(mTitle, this);
    dialog.setTypeRef(getReference());
    if (dialog.exec() != QDialog::Accepted)
      return;

    const auto ref = dialog.typeRef();
    if (ref.isValid())
      setReference(dialog.typeRef());
    else
      LOG_WARNING("Failed to set type ref");
  });

  containerLayout->addWidget(mSelector, 1, Qt::AlignVCenter);
  containerLayout->addWidget(targetButton, 0, Qt::AlignVCenter);
}

koda::types::TypeReference TypeSelector::getReference() const
{
  const QVariant data = mSelector->currentData();
  if (data.canConvert<koda::types::TypeReference>())
    return data.value<koda::types::TypeReference>();
  else
    return koda::types::TypeReference::named(koda::types::QualifiedName(mSelector->currentText().toStdString()));
}

void TypeSelector::setReference(const koda::types::TypeReference& reference)
{
  auto qname = QString::fromStdString(reference.toString());
  const int index = mSelector->findText(qname);
  if (index >= 0)
  {
    mSelector->setCurrentIndex(index);
    return;
  }

  mSelector->addItem(qname, QVariant::fromValue(reference));
  mSelector->setCurrentIndex(mSelector->count() - 1);
}

QComboBox* TypeSelector::createComboBox()
{
  auto* typeCombo = new QComboBox(this);
  typeCombo->setEditable(true);
  typeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  typeCombo->view()->setMinimumHeight(250);
  typeCombo->view()->setMaximumHeight(250);
  typeCombo->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  typeCombo->view()->verticalScrollBar()->setSingleStep(10);

  maki::addCompleter(maki::TypeRegistry::instance().allTypeNames(), typeCombo);

  connect(typeCombo->lineEdit(), &QLineEdit::editingFinished, this, [typeCombo]() {
    const QString text = typeCombo->currentText().trimmed();
    if (!TypeRegistry::instance().findByName(text.toStdString()))
      typeCombo->setCurrentIndex(-1);
  });

  return typeCombo;
}

void TypeSelector::setPrimitive()
{
  mUsesPrimitives = true;
  populateTypes();
}

void TypeSelector::setEditorTitle(const QString& title)
{
  mTitle = title;
}

void TypeSelector::populateTypes(const koda::types::QualifiedName* currentName)
{
  mSelector->clear();

  if (mUsesPrimitives)
  {
    for (const auto& def : maki::TypeRegistry::instance().allTypes())
      if (maki::TypeRegistry::instance().isBuiltin(*def) && (currentName == nullptr || def->name.toString() != currentName->toString()))
        setReference(def->toReference());
  }
  else
  {
    for (const auto& def : maki::TypeRegistry::instance().allTypes())
      if (currentName == nullptr || def->name.toString() != currentName->toString())
        setReference(def->toReference());
  }
}
}  // namespace maki
