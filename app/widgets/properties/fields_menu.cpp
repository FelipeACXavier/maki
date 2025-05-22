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
#include "app_configs.h"
#include "elements/node.h"
#include "json.h"
#include "logging.h"
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

VoidResult FieldsMenu::onNodeAdded(NodeItem* /* node */)
{
  return VoidResult();
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

VoidResult FieldsMenu::loadControls(NodeItem* node)
{
  if (node->controls().size() < 1)
  {
    QLabel* nameLabel = new QLabel("Block has no fields", this);
    nameLabel->setFont(Fonts::Property);

    layout()->setAlignment(Qt::AlignCenter);
    layout()->addWidget(nameLabel);

    return VoidResult();
  }

  // Controls are placed in a new horizontal widget at the bottom of the properties menu
  QWidget* controls = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(controls);

  controls->setLayout(controlLayout);
  layout()->addWidget(controls);

  for (const auto& control : node->controls())
  {
    if (control.type == Types::ControlTypes::ADD_FIELD)
      LOG_WARN_ON_FAILURE(loadControlAddField(control, node, controls, controlLayout));
    else if (control.type == Types::ControlTypes::ADD_EVENT)
      LOG_WARN_ON_FAILURE(loadControlAddEvent(control, node, controls, controlLayout));
    else
      LOG_WARNING("Unknown control type: %s", qPrintable(control.id));
  }

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

VoidResult FieldsMenu::loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
  QStandardItemModel* model = new QStandardItemModel(0, 3);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Type"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Return"));
  model->setHorizontalHeaderItem(3, new QStandardItem("Argument"));

  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableView->setContextMenuPolicy(Qt::CustomContextMenu);

  addDynamicWidget((QVBoxLayout*)layout(), tableView, parent);

  // We do not support editing values in the table directly. I want to avoid issues caused by a wrong click
  // Instead, we open a dialog with a complete overview of the event.
  // TODO(felaze): It would be nice to also show the nodes that trigger this event
  tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView->setModel(model);

  for (const auto& event : node->events())
  {
    int newRow = model->rowCount();
    addEventToTable(model, newRow, event);
  }

  connect(tableView, &QTableView::customContextMenuRequested, [this, tableView, node](const QPoint& pos) {
    showEventContextMenu(tableView, node, pos);
  });

  connect(tableView, &QTableView::doubleClicked, [this, tableView, node](const QModelIndex& index) {
    editEvent(tableView, node, index);
  });

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, [=]() {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(""));
    model->setItem(newRow, 1, new QStandardItem(""));
    model->setItem(newRow, 2, new QStandardItem(""));
    model->setItem(newRow, 3, new QStandardItem(""));
  });

  button->setText(control.id);
  controlLayout->addWidget(button);

  return VoidResult();
}

void FieldsMenu::showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos)
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

void FieldsMenu::editEvent(QTableView* tableView, NodeItem* node, const QModelIndex& index)
{
  // Open the dialog
  mCurrentDialog = new EventDialog("Edit event", this);

  EventConfig config = index.row() < node->events().size() ? node->events().at(index.row()) : EventConfig();
  mCurrentDialog->setup(config);

  connect(mCurrentDialog, &QDialog::accepted, [this, tableView, node, index] {
    int row = index.row();

    EventConfig event;
    event.id = mCurrentDialog->getName();
    event.type = Types::StringToConnectorType(mCurrentDialog->getType());
    event.returnType = Types::StringToPropertyTypes(mCurrentDialog->getReturnType());

    auto model = mCurrentDialog->getArguments();
    for (int i = 0; i < model->rowCount(); ++i)
    {
      PropertiesConfig property;
      property.id = model->index(i, 0).data().toString();
      property.type = Types::StringToPropertyTypes(model->index(i, 1).data().toString());
      event.arguments.push_back(property);
    }

    addEventToTable((QStandardItemModel*)tableView->model(), row, event);

    node->setEvent(row, event);
  });
  connect(mCurrentDialog, &QDialog::rejected, [this] {
    mCurrentDialog->close();
    mCurrentDialog->deleteLater();
  });

  mCurrentDialog->setAttribute(Qt::WA_DeleteOnClose);
  mCurrentDialog->show();
}

void FieldsMenu::addEventToTable(QStandardItemModel* model, int row, const EventConfig& event)
{
  model->setItem(row, 0, new QStandardItem(event.id));
  model->setItem(row, 1, new QStandardItem(Types::ConnectorTypeToString(event.type)));
  model->setItem(row, 2, new QStandardItem(Types::PropertyTypesToString(event.returnType)));

  if (event.arguments.isEmpty())
    return;

  QString args = "";
  for (const auto& arg : event.arguments)
    args += arg.id + ", ";

  // Remove the trailing ", "
  args.chop(2);
  model->setItem(row, 3, new QStandardItem(args));
}
