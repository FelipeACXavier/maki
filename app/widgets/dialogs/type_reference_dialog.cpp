#include "type_reference_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "../type_registry.h"
#include "../widget_factory.h"
#include "app_configs.h"

namespace maki
{

TypeReferenceDialog::TypeReferenceDialog(QWidget* parent)
    : BaseDialog(tr("Select Type"), 1.1, 0.2, parent)
    , mPrimitiveValue(defaultNamedType())
    , mNamedValue(defaultNamedType())
    , mListElement(defaultNamedType())
    , mMapKey(defaultNamedType())
    , mMapValue(defaultNamedType())
    , mOptionalElement(defaultNamedType())
{
  setModal(true);

  buildUi();
  setTypeRef(defaultNamedType());
  resize(420, sizeHint().height());
}

TypeReferenceDialog::SelectorButton TypeReferenceDialog::createTypeSelector(const QString& text, const QString& title, QWidget* parent,
                                                                            koda::types::TypeReference* refValue)
{
  auto* container = new QWidget(parent);
  auto containerAlignment = maki::WidgetAlignment{
      .type = maki::WidgetAlignment::Type::INLINE,
      .direction = maki::WidgetAlignment::Direction::SPREAD,
      .labelWidth = 50,
  };

  auto* containerLayout = new QHBoxLayout(container);
  containerLayout->setContentsMargins(0, 0, 0, 0);
  containerLayout->setAlignment(Qt::AlignTop);

  auto* selector = new maki::SelectorWidget(text, containerAlignment, container);
  for (const auto& item : maki::TypeRegistry::instance().allTypeNames())
    selector->addItem(item, item);

  selector->widget()->setMaximumWidth(100);
  containerLayout->addWidget(selector, 1, Qt::AlignTop);

  auto* button = new maki::ButtonWidget(tr("..."), container);
  button->setToolTip(tr("More types"));
  button->setIcon(QIcon(":/icons/bars.svg"));
  button->setFixedSize(Config::MEDIUM_BUTTON_SIZE);

  containerLayout->addWidget(selector, 0, Qt::AlignTop);
  containerLayout->addWidget(button, 1, Qt::AlignTop);

  connect(selector, &maki::SelectorWidget::dataChanged, [refValue](const QString& value, const QVariant& data) {
    if (data.canConvert<koda::types::TypeReference>())
      *refValue = data.value<koda::types::TypeReference>();
    else
      *refValue = koda::types::TypeReference::named(koda::types::QualifiedName(value.toStdString()));

    LOG_INFO("Setting type from selector: {}", refValue->toString());
  });
  connect(button, &maki::ButtonWidget::valueChanged, [this, title, selector, refValue] {
    if (!selector)
      return;

    *refValue = editNestedType(*refValue, title);

    const auto qname = QString::fromStdString(refValue->toString());
    selector->addItem(qname, QVariant::fromValue(*refValue));
    selector->setValue(qname);
    LOG_INFO("Setting type from button: {}", qname);
  });

  return {container, selector, button};
}

void TypeReferenceDialog::buildUi()
{
  // --------------------------------------------------------------------------
  // Type kind
  auto groupLayout = new maki::WidgetGroup(tr("Type selection"), this);

  auto alignment = maki::WidgetAlignment::Form(groupLayout, 75, maki::WidgetAlignment::Direction::SPREAD);
  mKindCombo = new maki::SelectorWidget(tr("Target type"), alignment, this);

  const auto primitivename = QString::fromStdString(koda::types::toString(koda::types::TypeReferenceKind::Primitive));
  mKindCombo->addItem(primitivename, primitivename);

  const auto namedname = QString::fromStdString(koda::types::toString(koda::types::TypeReferenceKind::Named));
  mKindCombo->addItem(namedname, namedname);

  const auto listname = QString::fromStdString(koda::types::toString(koda::types::TypeReferenceKind::List));
  mKindCombo->addItem(listname, listname);

  const auto mapname = QString::fromStdString(koda::types::toString(koda::types::TypeReferenceKind::Map));
  mKindCombo->addItem(mapname, mapname);

  const auto optname = QString::fromStdString(koda::types::toString(koda::types::TypeReferenceKind::Optional));
  mKindCombo->addItem(optname, optname);

  // --------------------------------------------------------------------------
  // Pages
  mStack = new QStackedWidget(this);

  // Primitive
  {
    auto group = new maki::WidgetGroup(tr("Primitive options"), oclero::qlementine::TextRole::H5, mStack);
    auto containerAlignment = maki::WidgetAlignment{
        .type = maki::WidgetAlignment::Type::INLINE,
        .direction = maki::WidgetAlignment::Direction::SPREAD,
    };

    mPrimitiveTypeCombo = new maki::SelectorWidget(tr("Type"), containerAlignment, group);
    for (int i = (int)koda::types::PrimitiveKind::Bool; i <= (int)koda::types::PrimitiveKind::Void; ++i)
      mPrimitiveTypeCombo->addItem(QString::fromStdString(koda::types::toString((koda::types::PrimitiveKind)i)), i);

    connect(mPrimitiveTypeCombo, &maki::SelectorWidget::dataChanged, [this](const QString& value, const QVariant& data) {
      mPrimitiveValue = koda::types::TypeReference::primitive((koda::types::PrimitiveKind)data.toInt());
    });

    group->addWidget(mPrimitiveTypeCombo);
    mStack->addWidget(group);
  }

  // Named
  {
    auto group = new maki::WidgetGroup(tr("Named options"), oclero::qlementine::TextRole::H5, mStack);

    auto container = createTypeSelector(tr("Type"), tr("Select named type"), group, &mNamedValue);
    mNamedTypeCombo = container.selector;
    group->addWidget(container.container);

    mStack->addWidget(group);
  }

  // List
  {
    auto group = new maki::WidgetGroup(tr("List options"), oclero::qlementine::TextRole::H5, mStack);

    auto container = createTypeSelector(tr("List type"), tr("Select list type"), group, &mListElement);

    mListElementCombo = container.selector;
    group->addWidget(container.container);

    mStack->addWidget(group);
  }

  // Map
  {
    auto group = new maki::WidgetGroup(tr("Map options"), oclero::qlementine::TextRole::H5, mStack);

    auto keyContainer = createTypeSelector(tr("Key type"), tr("Select map key type"), group, &mMapKey);
    auto valueContainer = createTypeSelector(tr("value type"), tr("Select map value type"), group, &mMapValue);

    mMapKeyCombo = keyContainer.selector;
    mMapValueCombo = valueContainer.selector;

    group->addWidget(keyContainer.container);
    group->addWidget(valueContainer.container);

    mStack->addWidget(group);
  }

  // Optional
  {
    auto group = new maki::WidgetGroup(tr("Optional options"), oclero::qlementine::TextRole::H5, mStack);

    auto container = createTypeSelector(tr("Contained type"), tr("Select optional type"), group, &mOptionalElement);
    mOptionalElementCombo = container.selector;
    group->addWidget(container.container);

    mStack->addWidget(group);
  }

  groupLayout->addWidget(mStack);

  layout()->addWidget(groupLayout);
  layout()->addWidget(mButtons);

  auto* buttonBox = createButtons(tr("Accept"), tr("Cancel"));

  connect(mKindCombo, &maki::SelectorWidget::valueChanged, this, &TypeReferenceDialog::kindChanged);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void TypeReferenceDialog::setTypeRef(const koda::types::TypeReference& ref)
{
  auto kindName = QString::fromStdString(koda::types::toString(ref.kind()));
  LOG_INFO("TypeReferenceDialog: {}", kindName);

  const int index = mKindCombo->widget()->findData(kindName);
  if (index >= 0)
    mKindCombo->setValue(kindName);

  switch (ref.kind())
  {
    case koda::types::TypeReferenceKind::Named:
    {
      if (ref.isNamed())
        mNamedValue = ref;
      else
        mNamedValue = defaultNamedType();

      updateText(mNamedValue, mNamedTypeCombo);
      break;
    }
    case koda::types::TypeReferenceKind::List:
    {
      if (ref.isList())
        mListElement = ref.elementType();
      else
        mListElement = defaultNamedType();

      updateText(mListElement, mListElementCombo);
      break;
    }
    case koda::types::TypeReferenceKind::Map:
    {
      const auto keyType = ref.mapKeyType();
      const auto valueType = ref.mapValueType();
      if (ref.isMap())
      {
        mMapKey = ref.mapKeyType();
        mMapValue = ref.mapValueType();
      }
      else
      {
        mMapKey = defaultNamedType();
        mMapValue = defaultNamedType();
      }

      updateText(mMapKey, mMapKeyCombo);
      updateText(mMapValue, mMapValueCombo);

      break;
    }
    case koda::types::TypeReferenceKind::Optional:
    {
      if (ref.isOptional())
        mOptionalElement = ref.optionalValueType();
      else
        mOptionalElement = defaultNamedType();

      updateText(mOptionalElement, mOptionalElementCombo);
      break;
    }
    case koda::types::TypeReferenceKind::Primitive:
    {
      if (ref.isPrimitive())
        mPrimitiveValue = ref;
      else
        mPrimitiveValue = defaultNamedType();

      mPrimitiveTypeCombo->setValue(QString::fromStdString(koda::types::toString(ref.primitiveKind())));
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
  switch (kind)
  {
    case koda::types::TypeReferenceKind::Named:
      return mNamedValue;
    case koda::types::TypeReferenceKind::List:
      return koda::types::TypeReference::list(mListElement);
    case koda::types::TypeReferenceKind::Map:
      return koda::types::TypeReference::map(mMapKey, mMapValue);
    case koda::types::TypeReferenceKind::Optional:
      return koda::types::TypeReference::optional(mOptionalElement);
    case koda::types::TypeReferenceKind::Primitive:
      return mPrimitiveValue;
    case koda::types::TypeReferenceKind::Unknown:
      break;
  }

  return defaultNamedType();
}

void TypeReferenceDialog::kindChanged(const QString& value)
{
  const auto kind = koda::types::typeReferenceKindFromString(value.toStdString());
  LOG_INFO("kindChanged: {} {}", value, (int)kind);
  switch (kind)
  {
    case koda::types::TypeReferenceKind::Named:
      mStack->setCurrentIndex(static_cast<int>(Page::Named));
      break;

    case koda::types::TypeReferenceKind::List:
      mStack->setCurrentIndex(static_cast<int>(Page::List));
      break;

    case koda::types::TypeReferenceKind::Map:
      mStack->setCurrentIndex(static_cast<int>(Page::Map));
      break;

    case koda::types::TypeReferenceKind::Optional:
      mStack->setCurrentIndex(static_cast<int>(Page::Optional));
      break;

    case koda::types::TypeReferenceKind::Primitive:
      mStack->setCurrentIndex(static_cast<int>(Page::Primitive));
      break;

    case koda::types::TypeReferenceKind::Unknown:
      return;
  }

  adjustSize();
}

koda::types::TypeReference TypeReferenceDialog::editNestedType(const koda::types::TypeReference& current, const QString& title)
{
  TypeReferenceDialog dialog(this);

  dialog.setWindowTitle(title);
  dialog.setTypeRef(current);

  if (dialog.exec() != QDialog::Accepted)
    return current;

  return dialog.typeRef();
}

void TypeReferenceDialog::updateText(const koda::types::TypeReference& ref, maki::SelectorWidget* widget)
{
  const auto val = QString::fromStdString(ref.toString());

  // Make sure the item is in the list
  widget->addItem(val, QVariant::fromValue(ref));
  widget->setValue(val);
}

koda::types::TypeReference TypeReferenceDialog::defaultNamedType()
{
  return koda::types::TypeReference::named(koda::types::StringType);
}

}  // namespace maki