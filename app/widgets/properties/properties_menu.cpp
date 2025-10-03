#include "properties_menu.h"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qsizepolicy.h>

#include <QScrollArea>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QStandardItemModel>
#include <QString>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <cfloat>

#include "../structure/event_dialog.h"
#include "../structure/field_dialog.h"
#include "app_configs.h"
#include "elements/node.h"
#include "elements/flow.h"
#include "keys.h"
#include "json.h"
#include "logging.h"
#include "style_helpers.h"
#include "types.h"

PropertiesMenu::PropertiesMenu(QWidget* parent)
    : QFrame(parent)
    , mCurrentNode("")
{
  // Set widget layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);
}

VoidResult PropertiesMenu::start(std::shared_ptr<SaveInfo> storage)
{
  mStorage = storage;
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

VoidResult PropertiesMenu::onNodeAdded(NodeItem* /* node */)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeRemoved(NodeItem* node)
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

VoidResult PropertiesMenu::onNodeModified(NodeItem* node)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeSelected(NodeItem* node, bool selected)
{
  // Clear the frame
  clear();

  if (!node)
    return VoidResult();

  mCurrentNode = node->id();

  RETURN_ON_FAILURE(loadProperties(node));
  RETURN_ON_FAILURE(loadControls(node));

  static_cast<QVBoxLayout*>(layout())->addStretch();

  return VoidResult();
}

VoidResult PropertiesMenu::onCreateEvent(NodeItem* node)
{
  // QWidget* w = this;  // or any child widget in the tab
  // QTabWidget* tabWidget = nullptr;

  // while (w)
  // {
  //   tabWidget = qobject_cast<QTabWidget*>(w);
  //   if (tabWidget)
  //     break;
  //   w = w->parentWidget();
  // }

  // if (tabWidget == nullptr)
  //   return VoidResult::Failed("Could not find parent widget");

  // tabWidget->setCurrentIndex(1);

  LOG_INFO("Creating event");
  onNodeSelected(node, true);

  auto table = findChild<QTableView*>("EventTable");
  if (table == nullptr)
    return VoidResult::Failed("Could not find the event table");

  openEventDialog(table, node, table->model()->rowCount() + 1);

  return VoidResult();
}

VoidResult PropertiesMenu::onFlowAdded(Flow* flow, NodeItem* node)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onFlowRemoved(const QString& flowId, NodeItem* node)
{
  return VoidResult();
}

VoidResult PropertiesMenu::loadProperties(NodeItem* node)
{
  if (node->configurationProperties().size() < 1)
    return VoidResult();

  for (const auto& property : node->configurationProperties())
  {
    // LOG_DEBUG("Updating properties with %s of type %d", qPrintable(property.id), (int)property.type);
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
    else if (property.type == Types::PropertyTypes::EVENT_SELECT)
      LOG_WARN_ON_FAILURE(loadPropertyEventSelect(property, node));
    else if (property.type == Types::PropertyTypes::COMPONENT_SELECT)
      LOG_WARN_ON_FAILURE(loadPropertyComponentSelect(property, node));
    else if (property.type == Types::PropertyTypes::STATE_SELECT)
      LOG_WARN_ON_FAILURE(loadPropertyStateSelect(property, node));
    else if (property.type == Types::PropertyTypes::SET_STATE)
      LOG_WARN_ON_FAILURE(loadPropertySetState(property, node));
    else
      LOG_WARNING("Property without a type, how is that possible?");
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadControls(NodeItem* node)
{
  if (node->controls().isEmpty() && node->events().isEmpty())
    return VoidResult();

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

VoidResult PropertiesMenu::loadPropertyInt(const PropertiesConfig& property, NodeItem* node)
{
  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
  QLineEdit* widget = new QLineEdit(this);
  // QIntValidator* validator = new QIntValidator(INT32_MIN, INT32_MIN, widget);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  // widget->setValidator(validator);

  connect(widget, &QLineEdit::editingFinished, this, [=]() {
    bool ok;
    int newValue = widget->text().toInt(&ok);
    if (ok)
      node->setProperty(property.id, newValue);
    else
      LOG_WARNING("Failed to set property of node %s to %d (%s)", qPrintable(node->nodeId()), newValue, qPrintable(widget->text()));
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyReal(const PropertiesConfig& property, NodeItem* node)
{
  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
  QLineEdit* widget = new QLineEdit(this);
  QDoubleValidator* validator = new QDoubleValidator(DBL_MIN, DBL_MAX, 6, widget);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  widget->setValidator(validator);

  connect(widget, &QLineEdit::returnPressed, this, [=]() {
    bool ok;
    qreal newValue = widget->text().toDouble(&ok);
    if (ok)
      node->setProperty(property.id, newValue);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyColor(const PropertiesConfig& property, NodeItem* node)
{
  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
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
  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
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
  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
  QLineEdit* widget = new QLineEdit(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  connect(widget, &QLineEdit::editingFinished, this, [=]() {
    node->setProperty(property.id, widget->text());
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);
  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node)
{
  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
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

VoidResult PropertiesMenu::loadPropertyStateSelect(const PropertiesConfig& property, NodeItem* node)
{
  // Get possible states
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QString label = ToLabel(property.id);
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);
  
  QComboBox* widget = new QComboBox(this);
  widget->setObjectName(property.id);

  auto callers = mStorage->getPossibleCallers(node->id());
  for (const auto& caller : callers)
  {
    auto callerName = caller->properties[ConfigKeys::NAME].toString();
    for (const auto& field : caller->fields)
    {
      if (field.type == Types::PropertyTypes::ENUM)
      {
        auto name = field.options.at(0).defaultValue.toString();
        auto values = field.options.at(1).options;

        for (const auto& opt : values)
        {
          QJsonObject data;
          data[ConfigKeys::ID] = caller->id;                                    // The UUID of the caller
          data[ConfigKeys::TYPE] = Types::PropertyTypesToString(field.type);    // Data type for convenience
          data[ConfigKeys::NAME] = callerName;                                  // The caller/component name
          data[ConfigKeys::FIELDS] = field.id;                                  // The field, e.g., State
          data[ConfigKeys::EVENTS] = opt.id;                                    // The field specifier, e.g., disabled
          data[ConfigKeys::DATA] = callerName + "." + field.id + "." + opt.id;  // generic.State.disabled

          widget->addItem(callerName + "." + field.id + "." + opt.id, data);
        }
      }
      else
      {
        QJsonObject data;
        data[ConfigKeys::ID] = caller->id;
        data[ConfigKeys::TYPE] = Types::PropertyTypesToString(field.type);
        data[ConfigKeys::NAME] = callerName;
        data[ConfigKeys::FIELDS] = field.id;
        data[ConfigKeys::DATA] = callerName + "." + field.id;

        widget->addItem(callerName + "." + field.id);
      }
    }
  }

  auto currentValue = node->getProperty(property.id);
  if (currentValue.isValid() && !currentValue.isNull())
  {
    auto value = currentValue.toJsonObject();
    widget->setCurrentText(value[ConfigKeys::DATA].toString());
  }

  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    auto data = widget->currentData();
    node->setProperty(property.id, data);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}


VoidResult PropertiesMenu::loadPropertySetState(const PropertiesConfig& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QGroupBox* group = new QGroupBox("Assignments", this);
  group->setObjectName("AssignmentGroup");
  group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  QVBoxLayout* groupLayout = new QVBoxLayout(group);

  QScrollArea* scrollArea = new QScrollArea(group);
  scrollArea->setWidgetResizable(true);

  // Create a scrollable content widget
  QWidget* scrollContent = new QWidget();
  scrollContent->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
  scrollLayout->setSpacing(5);        
  scrollLayout->setAlignment(Qt::AlignTop); 
  
  addStateAssignment(property, 0, node, scrollContent);

  scrollArea->setWidget(scrollContent);
  groupLayout->addWidget(scrollArea);
  qobject_cast<QVBoxLayout*>(layout())->addWidget(group, 1);

  return VoidResult();
}

void PropertiesMenu::addStateAssignment(const PropertiesConfig& property, int index, NodeItem* node, QWidget* group)
{
  QComboBox* widget = new QComboBox(group);
  widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  auto callers = mStorage->getPossibleStates(node->id());
  for (const auto& caller : callers)
  {
    QString callerName = "";
    if (!caller->parentId.isEmpty())
     callerName = caller->properties[ConfigKeys::NAME].toString() + ".";
    
    for (const auto& field : caller->fields)
      widget->addItem(callerName + field.id);
  }

  QWidget* controls = new QWidget(group);
  QHBoxLayout* controlsLayout = new QHBoxLayout(controls);

  QLabel* icon = new QLabel(controls);
  icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  icon->setPixmap(QPixmap(":/icons/equals.svg").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  icon->setFixedSize({20, 20});
  
  QLineEdit* assignEdit = new QLineEdit(group);
  assignEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  assignEdit->setFont(Fonts::Property);

  auto currentReturn = node->getProperty(property.id);
  bool addNew = false;
  if (currentReturn.isValid())
  {
    QJsonArray currentValue = currentReturn.toJsonArray();

    if (index < currentValue.size())
    {
      auto val = currentValue.at(index).toObject();
      if (val.contains("variable"))
        widget->setCurrentText(val["variable"].toString());
      if (val.contains("value"))
        assignEdit->setText(val["value"].toString());
    }

    addNew = index < currentValue.size() - 1;
  }
  
  connect(assignEdit, &QLineEdit::editingFinished, this, [=]{

    if (assignEdit->text().isEmpty())
    {
      controls->deleteLater();
      widget->deleteLater();
      return;
    }
    
    auto returnData = node->getProperty(property.id);
    if (!returnData.isValid())
    {
      LOG_WARNING("Invalid data in %s", qPrintable(property.id));
      return;
    }

    QJsonObject object;
    object["variable"] = widget->currentText();
    object["value"] = assignEdit->text();

    auto data = returnData.toJsonArray();
    if (index < data.size())
      data.replace(index, object);
    else
      data.append(object);

    node->setProperty(property.id, data);

    // Open a new assignment
    addStateAssignment(property, index + 1, node, group);
  });

  controlsLayout->addWidget(icon);
  controlsLayout->addWidget(assignEdit);
  
  group->layout()->addWidget(widget);
  group->layout()->addWidget(controls);

  if (addNew)
    addStateAssignment(property, index + 1, node, group);
}

// TODO(felaze): The component and the event fields are always dependent on each other for now
VoidResult PropertiesMenu::loadPropertyComponentSelect(const PropertiesConfig& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QComboBox* widget = new QComboBox(this);
  widget->setObjectName(property.id);

  for (const auto& child : mStorage->getPossibleCallers(node->id()))
  {
    auto name = child->properties[ConfigKeys::NAME];
    if (name.isNull() || !name.isValid())
      continue;

    widget->addItem(name.toString(), child->id);
  }

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  if (property.options.empty())
  {
    // Make sure the widget shows the current selected component if it exists
    auto currentValue = node->getProperty(property.id);
    if (currentValue.isValid())
      widget->setCurrentText(currentValue.toString());
    else
      widget->setCurrentText("-");

    connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
      node->setProperty(property.id, text);
    });
  }
  else
  {
    if (property.options.at(0).type == Types::PropertyTypes::EVENT_SELECT)
    {
      auto option = property.options.at(0);
      QComboBox* eventWidget = new QComboBox(this);
      eventWidget->setObjectName(option.id);

      // Set starting values
      auto value = node->getProperty(property.id);
      if (value.isValid())
      {
        QJsonObject object = value.toJsonObject();
        widget->setCurrentText(object[ConfigKeys::DATA].toString());

        auto events = mStorage->getEventsFromNode(widget->currentData().toString());
        for (const auto& event : events)
          eventWidget->addItem(event->name, event->id);

        eventWidget->setCurrentText(object[ConfigKeys::OPTION_DATA].toString());
      }
      else
      {
        widget->setCurrentText("-");
        // eventWidget->setCurrentText("-");
      }

      connect(eventWidget, &QComboBox::currentTextChanged, this, [node, property, eventWidget](const QString& text) {
        if (text.isEmpty())
          return;

        auto value = node->getProperty(property.id);
        if (!value.isValid())
          return;

        QJsonObject object = value.toJsonObject();
        object[ConfigKeys::OPTION_DATA] = text;
        object["option_data_id"] = eventWidget->currentData().toString();

        node->setProperty(property.id, object);
      });

      eventWidget->setFont(Fonts::Property);
      layout()->addWidget(eventWidget);

      connect(widget, &QComboBox::currentTextChanged, this, [this, widget, eventWidget, node, property](const QString& text) {
        if (text.isEmpty())
          return;

        eventWidget->clear();
        auto events = mStorage->getEventsFromNode(widget->currentData().toString());
        for (const auto& event : events)
          eventWidget->addItem(event->name, event->id);

        // Set the component
        auto value = node->getProperty(property.id);
        if (!value.isValid())
          return;

        QJsonObject object = value.toJsonObject();
        object[ConfigKeys::DATA] = text;
        object["data_id"] = widget->currentData().toString();
        object[ConfigKeys::OPTION_DATA] = events.size() > 0 ? events.at(0)->name : "";
        object["option_data_id"] = events.size() > 0 ? eventWidget->currentData().toString() : "";

        node->setProperty(property.id, object);
      });
    }
    else
    {
      LOG_WARNING("Configuration is not supported");
    }
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyEventSelect(const PropertiesConfig& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QComboBox* widget = new QComboBox(this);
  widget->setObjectName(property.id);

  // Make sure the widget shows the current selected component if it exists
  auto currentValue = node->getProperty(property.id);
  if (currentValue.isValid())
    widget->setCurrentText(currentValue.toString());
  else
    widget->setCurrentText("-");

  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    node->setProperty(property.id, text);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::onTransitionSelected(TransitionItem* transition)
{
  // Clear the frame
  clear();

  if (!transition)
    return VoidResult();

  mCurrentNode = transition->id();

  QLabel* nameLabel = new QLabel("Label");

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);

  QLineEdit* widget = new QLineEdit(this);
  widget->setText(transition->getName());
  connect(widget, &QLineEdit::returnPressed, this, [=]() {
    transition->setName(widget->text());
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  auto source = transition->source();
  if (source == nullptr)
    return VoidResult::Failed("Transition with no source");

  QLabel* comboLabel = new QLabel("Transition event");
  comboLabel->setFont(Fonts::Label);
  layout()->addWidget(comboLabel);

  QComboBox* eventWidget = new QComboBox(this);
  auto callers = mStorage->getPossibleCallers(source->id());
  for (const auto& caller : callers)
  {
    auto name = caller->properties[ConfigKeys::NAME];
    if (name.isNull() || !name.isValid())
      continue;

    auto events = mStorage->getEventsFromNode(caller->id);
    for (const auto& event : events)
      eventWidget->addItem(name.toString() + "." + event->name, event->id);

    eventWidget->setCurrentText(transition->getEvent());
  }

  connect(eventWidget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    transition->setEvent(text);
  });

  layout()->addWidget(eventWidget);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  return VoidResult();
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

VoidResult PropertiesMenu::loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  QString label = ToLabel(QStringLiteral("Events"));
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);

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

  for (const auto& event : node->events()) {
    LOG_INFO("Setting events for %s (%d): %s", qPrintable(node->nodeName()), model->rowCount(), qPrintable(event->name));
    addEventToTable(model, model->rowCount(), event);
  }

  connect(tableView, &QTableView::customContextMenuRequested, [this, tableView, node](const QPoint& pos) {
    showEventContextMenu(tableView, node, pos);
  });

  connect(tableView, &QTableView::doubleClicked, [this, tableView, node](const QModelIndex& index) {
    openEventDialog(tableView, node, index.row());
  });

  layout()->addWidget(tableView);
  if (control.id.isEmpty())
    return VoidResult();

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, [=]() {
    openEventDialog(tableView, node, model->rowCount());
  });

  button->setText(control.id);
  layout()->addWidget(button);

  return VoidResult();
}

VoidResult PropertiesMenu::loadControlAddState(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout)
{
  QString label = ToLabel(QStringLiteral("States"));
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);

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

void PropertiesMenu::showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos)
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

void PropertiesMenu::showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos)
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

void PropertiesMenu::openEventDialog(QTableView* tableView, NodeItem* node, int row)
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

void PropertiesMenu::openFieldDialog(QTableView* tableView, NodeItem* node, int row)
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

void PropertiesMenu::addEventToTable(QStandardItemModel* model, int row, std::shared_ptr<FlowSaveInfo> event)
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

void PropertiesMenu::addStateToTable(QStandardItemModel* model, int row, const PropertiesConfig& field)
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
