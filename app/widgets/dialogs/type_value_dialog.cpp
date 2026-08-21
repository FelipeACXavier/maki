#include "type_value_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "../dialogs/prompt.h"
#include "../properties/type_selector.h"
#include "../widget_factory.h"
#include "app_configs.h"
#include "style_helpers.h"
#include "type_reference_dialog.h"
#include "type_registry.h"

namespace maki
{

TypeValueDialog::TypeValueDialog(const QString& title, const MissionParameter& parameter, QWidget* parent)
    : BaseDialog(title, 0.8, 0.6, parent)
    , mParameter(parameter)
{
  setModal(true);

  clearLayout(layout());
  buildUi();
  layout()->activate();
  adjustSize();
}

void TypeValueDialog::buildUi()
{
  auto group = new maki::WidgetGroup(tr("Type selection"), this);
  maki::WidgetAlignment alignment = maki::WidgetAlignment::Form(group);

  // TODO: Add variable name validator
  mNameEditor = new maki::StringWidget(tr("Name"), QString::fromStdString(mParameter.name), alignment, group);
  mTypeEditor = new TypeSelector("parameterType", group);
  auto* containerLayout = new maki::ContainerWidget(tr("Target type"), mTypeEditor, alignment, group);
  Q_UNUSED(containerLayout);

  mValueGroup = new maki::WidgetScrollGroup(tr("Parameter value"), this);
  mValueGroup->setPadding(8);
  group->addWidget(mValueGroup);

  layout()->addWidget(group);

  mButtonBox = createButtons(tr("Accept"), tr("Cancel"));
  connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  connect(mNameEditor, &maki::StringWidget::valueChanged, this, [this](const QString& value) { mParameter.name = value.toStdString(); });
  connect(mTypeEditor, &maki::TypeSelector::typeChanged, this, [this](const koda::types::TypeReference& ref) {
    mParameter.type = ref;
    buildValueGroup(ref, Value::defaultValue(ref));
  });

  // Set the initial value
  if (mParameter.type.isValid())
  {
    mTypeEditor->setReference(mParameter.type);
  }
  else
  {
    mParameter.type = mTypeEditor->getReference();
    mParameter.value = Value::defaultValue(mParameter.type);
  }

  buildValueGroup(mParameter.type, mParameter.value);

  // Start focusing on the name editor
  mNameEditor->widget()->setFocus();
}

MissionParameter TypeValueDialog::getParameter() const
{
  return mParameter;
}

void TypeValueDialog::buildValueGroup(const koda::types::TypeReference& ref, const Value& value)
{
  // Clear previous widgets
  LOG_DEBUG("buildValueGroup START {}", ref.toString());
  mValueGroup->clear();

  LOG_DEBUG("Creating editor");
  // Make sure the value is updated
  mParameter.value = value;

  maki::WidgetAlignment valueAlignment = maki::WidgetAlignment::Form(mValueGroup);
  auto* editor = ValueEditorFactory::create("Value", ref, value, valueAlignment, mValueGroup);
  LOG_DEBUG("Editor created");
  if (!editor)
  {
    LOG_WARNING("Failed to create editor in value dialog");
    return;
  }

  connect(editor, &InputWidget::valueChanged, this, [this, editor] {
    mParameter.value = editor->getValue();
    LOG_DEBUG("Value of editor changed, updating: {} to {}", mParameter.name, mParameter.value.toReadable());
  });

  updateFocus(editor);
  LOG_DEBUG("buildValueGroup END");
}

void TypeValueDialog::updateFocus(maki::InputWidget* editor)
{
  QList<QWidget*> order;
  QList<QWidget*> focusable;
  if (mNameEditor)
    order.append(mNameEditor->focusWidgets());

  if (mTypeEditor)
    order.append(mTypeEditor->focusWidgets());

  if (editor)
  {
    focusable = editor->focusWidgets();
    order.append(focusable);
  }

  if (mButtonBox)
  {
    if (auto* ok = mButtonBox->button(QDialogButtonBox::Ok))
      order.append(ok);
    if (auto* cancel = mButtonBox->button(QDialogButtonBox::Close))
      order.append(cancel);
  }

  for (qsizetype i = 0; i + 1 < order.size(); ++i)
  {
    // LOG_DEBUG("Tab order: {} -> {}", order[i]->metaObject()->className(), order[i + 1]->metaObject()->className());
    setTabOrder(order[i], order[i + 1]);
  }

  // Close the loop
  if (order.size() > 1)
    setTabOrder(order.last(), order.first());
  if (!focusable.empty())
    focusable.first()->setFocus();
}

void TypeValueDialog::accept()
{
  LOG_DEBUG("Accepting: {} {} {}", mParameter.name, mParameter.type.toString(), koda::types::toString(mParameter.type.kind()));

  if (mParameter.name.empty())
  {
    if (mNameEditor)
      mNameEditor->widget()->setStatus(oclero::qlementine::Status::Error);

    if (maki::warningPrompt(tr("Missing name"), tr("The parameter will not be saved.")))
    {
      mParameter = {};
      QDialog::accept();
    }

    return;
  }

  // Check for the id in case
  if (!mParameter.type.isValid())
  {
    if (maki::warningPrompt(tr("Invalid type"), tr("The parameter will not be saved.")))
    {
      mParameter = {};
      QDialog::accept();
    }

    return;
  }

  QDialog::accept();
}

}  // namespace maki
