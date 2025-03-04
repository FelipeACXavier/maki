#include "dynamic_control.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>

#include "app_configs.h"

DynamicControl::DynamicControl(QWidget* parent)
    : QWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, this, &DynamicControl::onCustomContextMenuRequested);
}

VoidResult DynamicControl::setup(Types::ControlTypes type)
{
  if (type == Types::ControlTypes::ADD_FIELD)
    return setupAddField();

  return VoidResult::Failed("Unknown control type");
}

VoidResult DynamicControl::setupAddField()
{
  QHBoxLayout* addedControlLayout = new QHBoxLayout(this);
  setLayout(addedControlLayout);

  // Field type
  QLabel* fieldType = new QLabel(this);
  fieldType->setText("Type");
  addedControlLayout->addWidget(fieldType);

  QLineEdit* typeInput = new QLineEdit(this);
  typeInput->setFont(Fonts::Property);
  typeInput->setPlaceholderText("string");
  addedControlLayout->addWidget(typeInput);

  // Field name
  QLabel* fieldName = new QLabel(this);
  fieldName->setText("Name");
  addedControlLayout->addWidget(fieldName);

  QLineEdit* nameInput = new QLineEdit(this);
  nameInput->setFont(Fonts::Property);
  nameInput->setPlaceholderText("name");
  addedControlLayout->addWidget(nameInput);

  return VoidResult();
}

void DynamicControl::onCustomContextMenuRequested(const QPoint& pos)
{
  QMenu contextMenu(this);

  QAction* deleteAction = contextMenu.addAction("Remove");
  connect(deleteAction, &QAction::triggered, this, &DynamicControl::onRemoveRequested);

  contextMenu.exec(mapToGlobal(pos));
}

void DynamicControl::onRemoveRequested()
{
  // Get the parent layout or widget and remove the current widget
  QLayout* parentLayout = parentWidget()->layout();
  if (parentLayout)
  {
    parentLayout->removeWidget(this);
    deleteLater();
  }
}
