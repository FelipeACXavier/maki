#include "dynamic_control.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QStandardItemModel>
#include <QTableView>

#include "app_configs.h"
#include "elements/node.h"

DynamicControl::DynamicControl(Types::ControlTypes type, QWidget* parent)
    : QWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, this, &DynamicControl::onCustomContextMenuRequested);
}

VoidResult DynamicControl::setupAddField(NodeItem* node)
{
  QTableView* parent = qobject_cast<QTableView*>(parentWidget());
  if (!parent)
    return VoidResult::Failed("Parent is not a table");

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(parent->model());
  if (!parent)
    return VoidResult::Failed("Table does not follow a standard item model");

  int newRow = model->rowCount();
  model->insertRow(newRow);
  model->setItem(newRow, 0, new QStandardItem("name"));
  model->setItem(newRow, 1, new QStandardItem("value"));
  model->setItem(newRow, 2, new QStandardItem("type"));

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
