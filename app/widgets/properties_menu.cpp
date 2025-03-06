#include "properties_menu.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QString>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <cfloat>

#include "app_configs.h"
#include "dynamic_control.h"
#include "elements/node.h"
#include "json.h"
#include "logging.h"
#include "style_helpers.h"

PropertiesMenu::PropertiesMenu(QWidget* parent)
    : QFrame(parent)
{
  // Set widget layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);
}

VoidResult PropertiesMenu::onNodeSelected(NodeItem* node)
{
  // Clear the frame
  clear();

  RETURN_ON_FAILURE(loadProperties(node));

  static_cast<QVBoxLayout*>(layout())->addStretch();

  RETURN_ON_FAILURE(loadControls(node));

  return VoidResult();
}

void PropertiesMenu::clear()
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

VoidResult PropertiesMenu::loadProperties(NodeItem* node)
{
  for (const auto& property : node->configurationProperties())
  {
    // LOG_DEBUG("Updating properties with %s of type %d", qPrintable(property.id), (int)property.type);

    QString label = ToLabel(property.id);
    QLabel* nameLabel = new QLabel(label);

    nameLabel->setFont(Fonts::Label);
    layout()->addWidget(nameLabel);

    if (property.type == Types::PropertyTypes::STRING)
      LOG_WARN_ON_FAILURE(loadPropertyString(property, node));
    else if (property.type == Types::PropertyTypes::INTEGER)
      LOG_WARN_ON_FAILURE(loadPropertyInt(property, node));
    else if (property.type == Types::PropertyTypes::REAL)
      LOG_WARN_ON_FAILURE(loadPropertyReal(property, node));
    else if (property.type == Types::PropertyTypes::BOOLEAN)
      LOG_WARN_ON_FAILURE(loadPropertyBoolean(property, node));
    else if (property.type == Types::PropertyTypes::SELECT)
      LOG_WARN_ON_FAILURE(loadPropertySelect(property, node));
    else if (property.type == Types::PropertyTypes::COLOR)
      LOG_WARN_ON_FAILURE(loadPropertyColor(property, node));
    else
      LOG_WARNING("Property without a type, how is that possible?");
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyInt(const PropertiesConfig& property, NodeItem* node)
{
  QLineEdit* widget = new QLineEdit(this);
  QIntValidator* validator = new QIntValidator(INT32_MIN, INT32_MIN, widget);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  widget->setValidator(validator);

  connect(widget, &QLineEdit::textChanged, this, [=](const QString& text) {
    bool ok;
    int newValue = text.toInt(&ok);
    if (ok)
      node->setProperty(property.id, newValue);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyReal(const PropertiesConfig& property, NodeItem* node)
{
  QLineEdit* widget = new QLineEdit(this);
  QDoubleValidator* validator = new QDoubleValidator(DBL_MIN, DBL_MAX, 6, widget);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  widget->setValidator(validator);

  connect(widget, &QLineEdit::textChanged, this, [=](const QString& text) {
    bool ok;
    qreal newValue = text.toDouble(&ok);
    if (ok)
      node->setProperty(property.id, newValue);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyColor(const PropertiesConfig& property, NodeItem* node)
{
  QWidget* holder = new QWidget(this);
  QHBoxLayout* holderLayout = new QHBoxLayout(holder);
  holder->setLayout(holderLayout);

  QPushButton* widget = new QPushButton(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  QColor selectedColor = QColor::fromString(result.toString());
  QPalette palette;
  palette.setColor(QPalette::Window, selectedColor);

  QLabel* colorPreviewLabel = new QLabel(this);
  colorPreviewLabel->setAutoFillBackground(true);
  colorPreviewLabel->setPalette(palette);

  connect(widget, &QPushButton::pressed, [=]() {
    QColor color = QColorDialog::getColor(selectedColor, this, "Background Color");
    QPalette newPalette;
    newPalette.setColor(QPalette::Window, color);
    colorPreviewLabel->setPalette(newPalette);

    node->config()->body.backgroundColor = color;
    node->setProperty(property.id, color.name());
  });

  widget->setText(result.toString());
  widget->setFont(Fonts::Property);

  holderLayout->addWidget(colorPreviewLabel);
  holderLayout->addWidget(widget);

  layout()->addWidget(holder);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertySelect(const PropertiesConfig& property, NodeItem* node)
{
  QComboBox* widget = new QComboBox(this);

  auto options = property.options;
  for (const auto& option : options)
    widget->addItem(option.id);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setCurrentText(result.toString());
  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    node->setProperty(property.id, text);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyString(const PropertiesConfig& property, NodeItem* node)
{
  QLineEdit* widget = new QLineEdit(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  connect(widget, &QLineEdit::textChanged, this, [=](const QString& text) {
    node->setProperty(property.id, text);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);
  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node)
{
  QCheckBox* widget = new QCheckBox(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setChecked(result.toBool());
  connect(widget, &QCheckBox::checkStateChanged, this, [=](Qt::CheckState state) {
    node->setProperty(property.id, state);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadControls(NodeItem* node)
{
  if (node->controls().size() < 0)
    return VoidResult();

  // Controls are placed in a new horizontal widget at the bottom of the properties menu
  QWidget* controls = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(controls);

  controls->setLayout(controlLayout);
  layout()->addWidget(controls);

  for (const auto& control : node->controls())
  {
    if (control.type == Types::ControlTypes::ADD_FIELD)
      LOG_WARN_ON_FAILURE(loadControlAddField(control, node, controls, controlLayout));
    else
      LOG_WARNING("Unknown control type: %s", qPrintable(control.id));
  }

  return VoidResult();
}

void PropertiesMenu::addDynamicWidget(QWidget* dynamicWidget, QWidget* parent)
{
  // Add dynamic widgets above the parent but below other widgets
  for (int i = 0; i < layout()->count(); ++i)
  {
    QWidget* widget = layout()->itemAt(i)->widget();
    if (widget != parent)
      continue;

    static_cast<QVBoxLayout*>(layout())->insertWidget(i - 1, dynamicWidget);
    break;
  }
}

VoidResult PropertiesMenu::loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
  QStandardItemModel* model = new QStandardItemModel(0, 3);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Value"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Type"));

  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  addDynamicWidget(tableView, parent);

  tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  tableView->setModel(model);

  for (const auto& field : node->fields())
  {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(field.id));
    model->setItem(newRow, 2, new QStandardItem(field.typeToString()));

    if (field.type == Types::PropertyTypes::LIST)
      model->setItem(newRow, 1, new QStandardItem(JSON::fromArray(field.defaultValue.toList(), ',')));
    else
      model->setItem(newRow, 1, new QStandardItem(field.defaultValue.toString()));
  }

  QObject::connect(model, &QStandardItemModel::itemChanged, [=](QStandardItem* item) {
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

// tableView->setContextMenuPolicy(Qt::CustomContextMenu);
// connect(tableView, &QTableView::customContextMenuRequested, this, &YourClass::showContextMenu);
