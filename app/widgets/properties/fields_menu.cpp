#include "fields_menu.h"

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

#include "../structure/event_dialog.h"
#include "../structure/field_dialog.h"
#include "app_configs.h"
#include "config.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "json.h"
#include "logging.h"
#include "result.h"
#include "style_helpers.h"

FieldsMenu::FieldsMenu(QWidget* parent)
    : QFrame(parent)
    , mCurrentDialog(nullptr)
{
  // Set widget layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);
}

void FieldsMenu::clear()
{
  if (!layout())
    return;

  // Remove and delete all child widgets
  while (QLayoutItem* item = layout()->takeAt(0))
  {
    if (QWidget* widget = item->widget())
      widget->deleteLater();

    delete item;
  }
}

VoidResult FieldsMenu::onNodeAdded(NodeItem* node)
{
  return onNodeSelected(node, true);
}

VoidResult FieldsMenu::onNodeRemoved(NodeItem* node)
{
  if (!node)
    return VoidResult();

  // Clear the frame
  if (node->id() != mCurrentNode)
    return VoidResult();

  clear();
  mCurrentNode.clear();

  return VoidResult();
}

VoidResult FieldsMenu::onNodeModified(NodeItem* node)
{
  return VoidResult();
}

VoidResult FieldsMenu::onNodeSelected(NodeItem* node, bool selected)
{
  // Clear the frame
  clear();

  if (!node)
    return VoidResult();

  mCurrentNode = node->id();

  RETURN_ON_FAILURE(loadControls(node));

  return VoidResult();
}

VoidResult FieldsMenu::onCreateEvent(NodeItem* node)
{
  QWidget* w = this;  // or any child widget in the tab
  QTabWidget* tabWidget = nullptr;

  while (w)
  {
    tabWidget = qobject_cast<QTabWidget*>(w);
    if (tabWidget)
      break;
    w = w->parentWidget();
  }

  if (tabWidget == nullptr)
    return VoidResult::Failed("Could not find parent widget");

  tabWidget->setCurrentIndex(1);

  onNodeSelected(node, true);

  auto table = findChild<QTableView*>("EventTable");
  if (table == nullptr)
    return VoidResult::Failed("Could not find the event table");

  openEventDialog(table, node, table->model()->rowCount() + 1);

  return VoidResult();
}

VoidResult FieldsMenu::onFlowAdded(Flow* flow, NodeItem* node)
{
  return VoidResult();
}

VoidResult FieldsMenu::onFlowRemoved(const QString& flowId, NodeItem* node)
{
  return VoidResult();
}

VoidResult FieldsMenu::loadControls(NodeItem* node)
{
  if (node->controls().isEmpty() && node->events().isEmpty())
  {
    QLabel* nameLabel = new QLabel("Block has no events", this);
    nameLabel->setFont(Fonts::Property);

    layout()->setAlignment(Qt::AlignCenter);
    layout()->addWidget(nameLabel);

    return VoidResult();
  }

  QWidget* controls = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(controls);
  controls->setLayout(controlLayout);

  for (const auto& control : node->controls())
  {
    if (control.type == Types::ControlTypes::ADD_FIELD)
      LOG_WARN_ON_FAILURE(loadControlAddField(control, node, controls, controlLayout));
    else if (control.type == Types::ControlTypes::ADD_EVENT)
      LOG_WARN_ON_FAILURE(loadControlAddEvent(control, node, controls, controlLayout));
    else if (control.type == Types::ControlTypes::ADD_STATE)
      LOG_WARN_ON_FAILURE(loadControlAddState(control, node, controls, controlLayout));
    else
      LOG_WARNING("Unknown control type: %s", qPrintable(control.id));
  }

  if (!node->events().isEmpty() && node->controls().isEmpty())
    loadControlAddEvent(ControlsConfig(), node, controls, controlLayout);

  // Controls are placed in a new horizontal widget at the bottom of the properties menu
  layout()->addWidget(controls);

  return VoidResult();
}

VoidResult FieldsMenu::loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
  QStandardItemModel* model = new QStandardItemModel(0, 3);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Value"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Type"));

  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableView->setContextMenuPolicy(Qt::CustomContextMenu);

  addDynamicWidget((QVBoxLayout*)layout(), tableView, parent);

  tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  tableView->setModel(model);

  for (const auto& field : node->fields())
  {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(field.id));
    model->setItem(newRow, 2, new QStandardItem(Types::PropertyTypesToString(field.type)));

    if (field.type == Types::PropertyTypes::LIST)
      model->setItem(newRow, 1, new QStandardItem(JSON::fromArray(field.defaultValue.toList(), ',')));
    else
      model->setItem(newRow, 1, new QStandardItem(field.defaultValue.toString()));
  }

  connect(model, &QStandardItemModel::itemChanged, [=](QStandardItem* item) {
    if (!item)
      return;

    QJsonObject json;

    // TODO: clean this up and make the divider a configuration option
    int row = item->row();
    for (int i = 0; i < model->columnCount(); ++i)
    {
      if (!model->item(row, i))
        return;

      auto text = model->item(row, i)->text();
      if (text.isNull() || text.isEmpty())
        return;

      if (i == 0)
        json["id"] = text;
      else if (i == 1)
        json["default"] = text;
      else
        json["type"] = text;
    }

    if (json["type"] == "list")
      json["default"] = JSON::toArray(json["default"], ',');

    // LOG_INFO("Setting: %s %s %s", qPrintable(json["id"].toString()), qPrintable(json["default"].toString()), qPrintable(json["type"].toString()));
    LOG_ERROR_ON_FAILURE(node->setField(json["id"].toString(), json));
  });

  connect(tableView, &QTableView::customContextMenuRequested, [this, tableView, node](const QPoint& pos) {
    showContextMenu(tableView, node, pos);
  });

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, [=]() {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(""));
    model->setItem(newRow, 1, new QStandardItem(""));
    model->setItem(newRow, 2, new QStandardItem(""));
  });

  button->setText(control.id);
  controlLayout->addWidget(button);

  return VoidResult();
}

VoidResult FieldsMenu::loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
  tableView->setObjectName("EventTable");
  QStandardItemModel* model = new QStandardItemModel(0, 4);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Type"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Return"));
  model->setHorizontalHeaderItem(3, new QStandardItem("Argument"));

  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableView->setContextMenuPolicy(Qt::CustomContextMenu);

  // addDynamicWidget((QVBoxLayout*)layout(), tableView, parent);

  // We do not support editing values in the table directly. I want to avoid issues caused by a wrong click
  // Instead, we open a dialog with a complete overview of the event.
  // TODO(felaze): It would be nice to also show the nodes that trigger this event
  tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView->setModel(model);

  for (const auto& event : node->events())
    addEventToTable(model, model->rowCount(), event);

  connect(tableView, &QTableView::customContextMenuRequested, [this, tableView, node](const QPoint& pos) {
    showEventContextMenu(tableView, node, pos);
  });

  connect(tableView, &QTableView::doubleClicked, [this, tableView, node](const QModelIndex& index) {
    openEventDialog(tableView, node, index.row());
  });

  if (control.id.isEmpty())
    return VoidResult();

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, [=]() {
    openEventDialog(tableView, node, model->rowCount());
  });

  button->setText(control.id);
  layout()->addWidget(tableView);
  layout()->addWidget(button);

  return VoidResult();
}

VoidResult FieldsMenu::loadControlAddState(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
  tableView->setObjectName("StateTable");
  QStandardItemModel* model = new QStandardItemModel(0, 3);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Type"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Value"));

  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableView->setContextMenuPolicy(Qt::CustomContextMenu);

  // addDynamicWidget((QVBoxLayout*)layout(), tableView, parent);

  // We do not support editing values in the table directly. I want to avoid issues caused by a wrong click
  // Instead, we open a dialog with a complete overview of the event.
  // TODO(felaze): It would be nice to also show the nodes that trigger this event
  // tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView->setModel(model);

  for (const auto& field : node->fields())
    addStateToTable(model, model->rowCount(), field);

  connect(tableView, &QTableView::doubleClicked, [this, tableView, node](const QModelIndex& index) {
    openFieldDialog(tableView, node, index.row());
  });

  connect(tableView, &QTableView::customContextMenuRequested, [this, tableView, node](const QPoint& pos) {
    showContextMenu(tableView, node, pos);
  });

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, [=]() {
    openFieldDialog(tableView, node, model->rowCount());
  });

  button->setText(control.id);
  layout()->addWidget(tableView);
  layout()->addWidget(button);

  return VoidResult();
}

void FieldsMenu::showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos)
{
  // Get the index of the clicked row
  QModelIndex index = tableView->indexAt(pos);
  if (!index.isValid())
    return;

  QMenu contextMenu;
  QAction* actionDelete = contextMenu.addAction("Delete");

  int row = index.row();
  connect(actionDelete, &QAction::triggered, this, [row, tableView, node] {
    auto key = static_cast<QStandardItemModel*>(tableView->model())->item(row, 0);
    if (key && !key->text().isNull())
      node->removeField(key->text());

    tableView->model()->removeRow(row);
  });

  contextMenu.exec(tableView->viewport()->mapToGlobal(pos));
}

void FieldsMenu::showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos)
{
  // Get the index of the clicked row
  QModelIndex index = tableView->indexAt(pos);
  if (!index.isValid())
    return;

  int row = index.row();

  auto model = static_cast<QStandardItemModel*>(tableView->model());
  auto modifiable = model->item(row)->data(Qt::UserRole + 1).toBool();

  QMenu contextMenu;
  QAction* actionEditFlow = contextMenu.addAction(tr("Edit flow"));
  actionEditFlow->setEnabled(modifiable);
  connect(actionEditFlow, &QAction::triggered, this, [this, row, model, node] {
    auto flowId = model->item(row)->data(Qt::UserRole).toString();
    emit flowSelected(flowId, node->id());
  });

  QAction* actionEditProperties = contextMenu.addAction(tr("Edit event"));
  actionEditProperties->setEnabled(modifiable);
  connect(actionEditProperties, &QAction::triggered, this, [this, row, tableView, node] {
    openEventDialog(tableView, node, row);
  });

  QAction* actionDelete = contextMenu.addAction(tr("Delete"));
  connect(actionDelete, &QAction::triggered, this, [row, tableView, node] {
    auto key = static_cast<QStandardItemModel*>(tableView->model())->item(row, 0);
    if (key && !key->text().isNull())
      node->removeField(key->text());

    tableView->model()->removeRow(row);
  });

  contextMenu.exec(tableView->viewport()->mapToGlobal(pos));
}

void FieldsMenu::openEventDialog(QTableView* tableView, NodeItem* node, int row)
{
  // Open the dialog
  mCurrentDialog = new EventDialog(tr("Edit event"), this);

  auto config = row < node->events().size() ? node->events().at(row) : std::make_shared<FlowSaveInfo>();
  qobject_cast<EventDialog*>(mCurrentDialog)->setup(config);

  connect(mCurrentDialog, &QDialog::accepted, [this, tableView, node, row] {
    auto info = qobject_cast<EventDialog*>(mCurrentDialog)->getInfo();
    Flow* flow = node->createFlow(info->name, info);
    addEventToTable((QStandardItemModel*)tableView->model(), row, info);
    if (info->modifiable)
      emit flowSelected(flow->id(), node->id());
  });
  connect(mCurrentDialog, &QDialog::rejected, [this] {
    mCurrentDialog->close();
    mCurrentDialog->deleteLater();
  });

  mCurrentDialog->setAttribute(Qt::WA_DeleteOnClose);
  mCurrentDialog->exec();
}

void FieldsMenu::openFieldDialog(QTableView* tableView, NodeItem* node, int row)
{
  // Open the dialog
  mCurrentDialog = new FieldDialog(tr("Edit field"), this);

  auto config = row < node->fields().size() ? node->fields().at(row) : PropertiesConfig();
  qobject_cast<FieldDialog*>(mCurrentDialog)->setup(config);

  connect(mCurrentDialog, &QDialog::accepted, [this, tableView, node, row] {
    auto info = qobject_cast<FieldDialog*>(mCurrentDialog)->getInfo();
    node->setField(info.id, info);
    addStateToTable((QStandardItemModel*)tableView->model(), row, info);
  });
  connect(mCurrentDialog, &QDialog::rejected, [this] {
    mCurrentDialog->close();
    mCurrentDialog->deleteLater();
  });

  mCurrentDialog->setAttribute(Qt::WA_DeleteOnClose);
  mCurrentDialog->exec();
}

void FieldsMenu::addEventToTable(QStandardItemModel* model, int row, std::shared_ptr<FlowSaveInfo> event)
{
  // If the row is not in the table yet, add it
  if (row >= model->rowCount())
    model->insertRow(row);

  auto indexItem = new QStandardItem(event->name);
  indexItem->setData(event->id, Qt::UserRole);
  indexItem->setData(event->modifiable, Qt::UserRole + 1);

  model->setItem(row, 0, indexItem);
  model->setItem(row, 1, new QStandardItem(Types::ConnectorTypeToString(event->type)));
  model->setItem(row, 2, new QStandardItem(Types::PropertyTypesToString(event->returnType)));

  if (event->arguments.isEmpty())
    return;

  QString args = "";
  for (const auto& arg : event->arguments)
    args += arg.id + ", ";

  // Remove the trailing ", "
  args.chop(2);
  model->setItem(row, 3, new QStandardItem(args));
}

void FieldsMenu::addStateToTable(QStandardItemModel* model, int row, const PropertiesConfig& field)
{
  if (row >= model->rowCount())
    model->insertRow(row);

  model->setItem(row, 0, new QStandardItem(field.id));
  model->setItem(row, 1, new QStandardItem(Types::PropertyTypesToString(field.type)));

  if (field.type == Types::PropertyTypes::LIST)
    model->setItem(row, 2, new QStandardItem(JSON::fromArray(field.defaultValue.toList(), ',')));
  else
    model->setItem(row, 2, new QStandardItem(field.defaultValue.toString()));
}
