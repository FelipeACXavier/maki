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
  mSelector->setFocusPolicy(Qt::StrongFocus);
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
    {
      setReference(dialog.typeRef());
      emit typeChanged(dialog.typeRef());
    }
    else
    {
      LOG_WARNING("Failed to set type ref");
    }
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

  connect(typeCombo->lineEdit(), &QLineEdit::textEdited, this, [this](const QString& text) {
    const QString trimmed = text.trimmed();
    auto def = TypeRegistry::instance().findByName(trimmed.toStdString());
    if (def)
      emit typeChanged(def->toReference());
  });
  connect(typeCombo, &QComboBox::activated, this, [this, typeCombo](int index) {
    if (index < 0)
      return;

    const QVariant data = typeCombo->itemData(index);
    if (data.canConvert<koda::types::TypeReference>())
      emit typeChanged(data.value<koda::types::TypeReference>());
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

  mSelector->setCurrentIndex(0);
}

void TypeSelector::focusInEvent(QFocusEvent* event)
{
  QWidget::focusInEvent(event);

  if (mSelector)
    mSelector->setFocus(event->reason());
}

QList<QWidget*> TypeSelector::focusWidgets() const
{
  if (!mSelector)
    return {};

  return {mSelector};
}

}  // namespace maki
