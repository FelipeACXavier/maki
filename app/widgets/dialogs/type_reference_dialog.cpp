#include "type_reference_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

#include "../properties/type_selector.h"
#include "../type_registry.h"
#include "../widget_factory.h"
#include "app_configs.h"

namespace maki
{

TypeReferenceDialog::TypeReferenceDialog(const QString& title, QWidget* parent)
    : BaseDialog(title, 1.6, 0.2, parent)
{
  setModal(true);

  buildUi();
  setTypeRef(defaultNamedType());

  layout()->activate();
  adjustSize();
}

void TypeReferenceDialog::buildUi()
{
  auto groupLayout = new maki::WidgetGroup(tr("Type selection"), this);

  // --------------------------------------------------------------------------
  // Type kind
  {
    auto alignment = maki::WidgetAlignment::Form(groupLayout, Config::LONG_LABEL_WIDTH, maki::WidgetAlignment::Direction::SPREAD);
    mKindCombo = new maki::SelectorWidget(tr("Target type"), alignment, this);

    using Kind = koda::types::TypeReferenceKind;
    for (int i = (int)Kind::Primitive; i <= (int)Kind::Map; ++i)
    {
      const auto qname = QString::fromStdString(koda::types::toString((Kind)i));
      mKindCombo->addItem(qname, qname);
    }
  }

  // --------------------------------------------------------------------------
  // Pages
  {
    mStack = new QStackedWidget(this);

    // Primitive
    {
      auto group = new maki::WidgetGroup(tr("Primitive options"), oclero::qlementine::TextRole::H5, mStack);
      auto alignment = maki::WidgetAlignment::Form(group, Config::LONG_LABEL_WIDTH);
      auto selector = new maki::TypeSelector("primitiveSelector", group);
      selector->setEditorTitle(tr("Primitive type"));
      selector->setPrimitive();

      auto* containerLayout = new maki::ContainerWidget(tr("Primitive"), selector, alignment, group);
      Q_UNUSED(containerLayout);

      mStack->addWidget(group);
    }

    // Named
    {
      auto group = new maki::WidgetGroup(tr("Named options"), oclero::qlementine::TextRole::H5, mStack);

      auto alignment = maki::WidgetAlignment::Form(group, Config::LONG_LABEL_WIDTH);
      auto selector = new maki::TypeSelector("namedSelector", group);
      selector->setEditorTitle(tr("Named type"));
      auto* containerLayout = new maki::ContainerWidget(tr("Type"), selector, alignment, group);
      Q_UNUSED(containerLayout);

      mStack->addWidget(group);
    }

    // List
    {
      auto group = new maki::WidgetGroup(tr("List options"), oclero::qlementine::TextRole::H5, mStack);

      auto alignment = maki::WidgetAlignment::Form(group, Config::LONG_LABEL_WIDTH);
      auto selector = new maki::TypeSelector("listSelector", group);
      selector->setEditorTitle(tr("List type"));
      auto* containerLayout = new maki::ContainerWidget(tr("List type"), selector, alignment, group);
      Q_UNUSED(containerLayout);

      mStack->addWidget(group);
    }

    // Optional
    {
      auto group = new maki::WidgetGroup(tr("Optional options"), oclero::qlementine::TextRole::H5, mStack);

      auto alignment = maki::WidgetAlignment::Form(group, Config::LONG_LABEL_WIDTH);
      auto selector = new maki::TypeSelector("optionalSelector", group);
      selector->setEditorTitle(tr("Optional type"));
      auto* containerLayout = new maki::ContainerWidget(tr("Optional type"), selector, alignment, group);
      Q_UNUSED(containerLayout);

      mStack->addWidget(group);
    }

    // Map
    {
      auto group = new maki::WidgetGroup(tr("Map options"), oclero::qlementine::TextRole::H5, mStack);

      auto alignment = maki::WidgetAlignment::Form(group, Config::LONG_LABEL_WIDTH);
      auto keySelector = new maki::TypeSelector("keySelector", group);
      keySelector->setEditorTitle(tr("Key type"));
      auto valueSelector = new maki::TypeSelector("valueSelector", group);
      valueSelector->setEditorTitle(tr("Value type"));

      auto* k = new maki::ContainerWidget(tr("Key type"), keySelector, alignment, group);
      auto* v = new maki::ContainerWidget(tr("Value type"), valueSelector, alignment, group);
      Q_UNUSED(k);
      Q_UNUSED(v);

      mStack->addWidget(group);
    }

    groupLayout->addWidget(mStack);
  }

  layout()->addWidget(groupLayout);

  auto* buttonBox = createButtons(tr("Accept"), tr("Cancel"));
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mKindCombo, &maki::SelectorWidget::valueChanged, this, &TypeReferenceDialog::kindChanged);
}

void TypeReferenceDialog::setTypeRef(const koda::types::TypeReference& ref)
{
  const auto kind = ref.kind();
  auto kindName = QString::fromStdString(koda::types::toString(kind));
  mKindCombo->setValue(kindName);

  auto* page = mStack->widget(kindToIndex(kind));
  switch (kind)
  {
    case koda::types::TypeReferenceKind::Primitive:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("primitiveSelector");
      if (!selector)
        return;

      selector->setReference(ref);
      break;
    }
    case koda::types::TypeReferenceKind::Named:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("namedSelector");
      if (!selector)
        return;

      selector->setReference(ref);
      break;
    }
    case koda::types::TypeReferenceKind::List:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("listSelector");
      if (!selector)
        return;

      selector->setReference(ref);
      break;
    }
    case koda::types::TypeReferenceKind::Map:
    {
      auto* kselector = page->findChild<maki::TypeSelector*>("keySelector");
      auto* vselector = page->findChild<maki::TypeSelector*>("valueSelector");
      if (!kselector || !vselector)
        return;

      kselector->setReference(ref.mapKeyType());
      vselector->setReference(ref.mapValueType());
      break;
    }
    case koda::types::TypeReferenceKind::Optional:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("optionalSelector");
      if (!selector)
        return;

      selector->setReference(ref);
      break;
    }
    case koda::types::TypeReferenceKind::Unknown:
      break;
  }

  kindChanged(kindName);
}

koda::types::TypeReference TypeReferenceDialog::typeRef() const
{
  const auto kind = koda::types::typeReferenceKindFromString(mKindCombo->getValue().toStdString());
  auto* page = mStack->widget(kindToIndex(kind));
  switch (kind)
  {
    case koda::types::TypeReferenceKind::Primitive:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("primitiveSelector");
      if (!selector)
        return koda::types::TypeReference{};

      return selector->getReference();
    }
    case koda::types::TypeReferenceKind::Named:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("namedSelector");
      if (!selector)
        return koda::types::TypeReference{};

      return selector->getReference();
    }
    case koda::types::TypeReferenceKind::List:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("listSelector");
      if (!selector)
        return koda::types::TypeReference{};

      return koda::types::TypeReference::list(selector->getReference());
    }
    case koda::types::TypeReferenceKind::Map:
    {
      auto* kselector = page->findChild<maki::TypeSelector*>("keySelector");
      auto* vselector = page->findChild<maki::TypeSelector*>("valueSelector");
      if (!kselector || !vselector)
        return koda::types::TypeReference{};

      return koda::types::TypeReference::map(kselector->getReference(), vselector->getReference());
    }
    case koda::types::TypeReferenceKind::Optional:
    {
      auto* selector = page->findChild<maki::TypeSelector*>("optionalSelector");
      if (!selector)
        return koda::types::TypeReference{};

      return koda::types::TypeReference::optional(selector->getReference());
    }
    case koda::types::TypeReferenceKind::Unknown:
      break;
  }

  return defaultNamedType();
}

void TypeReferenceDialog::kindChanged(const QString& value)
{
  const auto kind = koda::types::typeReferenceKindFromString(value.toStdString());
  mStack->setCurrentIndex(kindToIndex(kind));
  adjustSize();
}

koda::types::TypeReference TypeReferenceDialog::defaultNamedType()
{
  return koda::types::TypeReference::named(koda::types::StringType);
}

int TypeReferenceDialog::kindToIndex(koda::types::TypeReferenceKind type) const
{
  return std::max(0, static_cast<int>(type) - 1);
}

}  // namespace maki