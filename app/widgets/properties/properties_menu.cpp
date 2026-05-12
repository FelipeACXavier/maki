#include "properties_menu.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardItemModel>
#include <QString>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <cfloat>

#include "../structure/event_dialog.h"
#include "../structure/field_dialog.h"
#include "app_configs.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "json.h"
#include "keys.h"
#include "logging.h"
#include "result.h"
#include "style_helpers.h"
#include "types.h"
#include "widgets/widget_factory.h"

static const int EVENT_INDEX = 0;
static const int ARG_INDEX = 1;
static const int CLEAR_INDEX = INT32_MAX;

#define UPDATE_PROPERTY(node, id, value)                  \
  do                                                      \
  {                                                       \
    auto propValue = node->getProperty(property.getid()); \
    if (!propValue.isValid())                             \
    {                                                     \
      LOG_WARNING("Property is not valid");               \
      return;                                             \
    }                                                     \
                                                          \
    QJsonObject object = propValue.toJsonObject();        \
    object[ConfigKeys::DATA] = text;                      \
    object[ConfigKeys::OPTIONS] = QJsonArray();           \
                                                          \
    node->setProperty(property.getid(), object);          \
  } while (false);

#define UPDATE_PROPERTY_ARG(node, id, index, value, type, isVariable) \
  do                                                                  \
  {                                                                   \
    auto propValue = node->getProperty(id);                           \
    if (propValue.isValid())                                          \
    {                                                                 \
      QJsonObject object = propValue.toJsonObject();                  \
      QJsonArray array = object[ConfigKeys::OPTIONS].toArray();       \
                                                                      \
      QJsonObject item;                                               \
      item[ConfigKeys::DATA] = value;                                 \
      item[ConfigKeys::TYPE] = Types::PropertyTypesToString(type);    \
      item[ConfigKeys::IS_VARIABLE] = isVariable;                     \
      if (index == CLEAR_INDEX)                                       \
        array = QJsonArray();                                         \
                                                                      \
      if (index < array.size())                                       \
        array[index] = item;                                          \
      else                                                            \
        array.push_back(item);                                        \
                                                                      \
      object[ConfigKeys::OPTIONS] = array;                            \
      node->setProperty(id, object);                                  \
    }                                                                 \
    else                                                              \
    {                                                                 \
      LOG_WARNING("Property is not valid");                           \
    }                                                                 \
  } while (false);

#define ADD_COMPLETER(FIELD, NODE, TYPE)                         \
  do                                                             \
  {                                                              \
    QStringList variables = {};                                  \
    auto parentStates = mStorage->getPossibleStates(NODE->id()); \
    for (const auto& state : parentStates)                       \
    {                                                            \
      if (!state)                                                \
        continue;                                                \
                                                                 \
      if (state->gettype() == TYPE)                              \
        variables.append(state->getid());                        \
    }                                                            \
                                                                 \
    QCompleter* completer = new QCompleter(variables, FIELD);    \
    completer->setCompletionMode(QCompleter::PopupCompletion);   \
    completer->setCaseSensitivity(Qt::CaseInsensitive);          \
    FIELD->widget()->setCompleter(completer);                    \
  } while (false);

PropertiesMenu::PropertiesMenu(QWidget* parent)
    : QFrame(parent)
    , mCurrentNode("")
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (qlementineStyle)
  {
    const auto theme = qlementineStyle->theme();

    // Set widget layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(theme.spacing, theme.spacing, 0, 0);
    layout->setSpacing(theme.spacing);

    mFrame = new StyledFrame(this);
    mFrame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    mFrame->setBorderRole(StyledFrame::BorderRole::Mid);
    mFrame->setRadius(theme.borderRadius);
    mFrame->setBorderWidth(theme.borderWidth);
    mFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    QVBoxLayout* frameLayout = new QVBoxLayout(mFrame);
    frameLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
    frameLayout->setSpacing(theme.spacing);

    layout->addWidget(mFrame);
  }
}

QLayout* PropertiesMenu::layout() const
{
  return mFrame ? mFrame->layout() : QFrame::layout();
}

VoidResult PropertiesMenu::start(std::shared_ptr<SaveInfo> storage)
{
  mStorage = storage;
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeAdded(NodeItem* /* node */)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeRemoved(const QString& nodeId)
{
  // Clear the frame
  if (nodeId != mCurrentNode)
    return VoidResult();

  clearLayout(layout());
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
  clearLayout(layout());

  if (!node)
    return VoidResult();

  mCurrentNode = node->id();

  RETURN_ON_FAILURE(loadProperties(node));
  RETURN_ON_FAILURE(loadControls(node));

  qobject_cast<QVBoxLayout*>(layout())->addStretch();

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
    else if (property.type == Types::PropertyTypes::LIST)
      continue;
    else if (property.type == Types::PropertyTypes::VOID)
      continue;
    else
      LOG_WARNING("Property %s (%d) without a type, how is that possible?", qPrintable(property.id), (int)property.type);
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

VoidResult PropertiesMenu::loadPropertyInt(const PropertyInfo& property, NodeItem* node)
{
  auto result = node->getProperty(property.getid());
  if (result.isNull())
    return VoidResult::Failed("Failed to get property " + property.getid().toStdString() + " of " + node->nodeName().toStdString());

  maki::WidgetAlignment alignment = {maki::WidgetAlignment::Type::VERTICAL};
  auto widget = new maki::IntegerWidget(ToLabel(property.getid()), "", alignment, this);
  if (result.isValid())
    widget->setValue(result.toInt());

  connect(widget, &maki::IntegerWidget::valueChanged, this, [node, property](const int value) {
    node->setProperty(property.getid(), value);
  });

  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyReal(const PropertyInfo& property, NodeItem* node)
{
  auto result = node->getProperty(property.getid());
  if (result.isNull())
    return VoidResult::Failed("Failed to get property " + property.getid().toStdString() + " of " + node->nodeName().toStdString());

  maki::WidgetAlignment alignment = {maki::WidgetAlignment::Type::VERTICAL};
  auto widget = new maki::FloatWidget(ToLabel(property.getid()), "", alignment, this);
  if (result.isValid())
    widget->setValue(result.toInt());

  connect(widget, &maki::FloatWidget::valueChanged, this, [node, property](const qreal value) {
    node->setProperty(property.getid(), value);
  });

  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyColor(const PropertyInfo& property, NodeItem* node)
{
  QString label = ToLabel(property.getid());
  QLabel* nameLabel = new QLabel(label);
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  QWidget* holder = new QWidget(this);
  QHBoxLayout* holderLayout = new QHBoxLayout(holder);
  holderLayout->setSpacing(10);
  holderLayout->setContentsMargins(5, 0, 5, 0);
  holder->setLayout(holderLayout);

  QPushButton* widget = new QPushButton(this);
  auto result = node->getProperty(property.getid());
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  QColor selectedColor = QColor::fromString(result.toString());

  QLabel* colorPreviewLabel = new QLabel(this);
  colorPreviewLabel->setFixedSize({widget->height(), widget->height()});
  colorPreviewLabel->setObjectName("PropertyColorPreview");
  applyStyle(colorPreviewLabel, QStringLiteral(
                                    "QLabel#PropertyColorPreview { background-color: %1; }")
                                    .arg(result.toString()));

  connect(widget, &QPushButton::pressed, [this, node, colorPreviewLabel, property, selectedColor]() {
    QColor color = QColorDialog::getColor(selectedColor, this, "Background Color");
    if (!color.isValid())
      return;

    applyStyle(colorPreviewLabel, QStringLiteral(
                                      "QLabel#PropertyColorPreview { background-color: %1; }")
                                      .arg(color.name()));

    node->setProperty(property.getid(), color.name());
    colorPreviewLabel->update();
  });

  widget->setText(result.toString());
  widget->setFont(Fonts::Property);

  holderLayout->addWidget(colorPreviewLabel);
  holderLayout->addWidget(widget);

  layout()->addWidget(nameLabel);
  layout()->addWidget(holder);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertySelect(const PropertyInfo& property, NodeItem* node)
{
  QString label = ToLabel(property.getid());
  QLabel* nameLabel = new QLabel(label);
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  QComboBox* widget = new QComboBox(this);

  auto options = property.getoptions();
  for (const auto& option : options)
    widget->addItem(option->getid());

  auto result = node->getProperty(property.getid());
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setCurrentText(result.toString());
  widget->setFont(Fonts::Property);

  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    node->setProperty(property.getid(), text);
  });

  layout()->addWidget(nameLabel);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyString(const PropertyInfo& property, NodeItem* node)
{
  QString label = ToLabel(property.getid());
  QLabel* nameLabel = new QLabel(label);
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  QLineEdit* widget = new QLineEdit(this);
  widget->setFont(Fonts::Property);

  auto result = node->getProperty(property.getid());
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  connect(widget, &QLineEdit::editingFinished, this, [=]() {
    node->setProperty(property.getid(), widget->text());
  });

  layout()->addWidget(nameLabel);
  layout()->addWidget(widget);
  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyBoolean(const PropertyInfo& property, NodeItem* node)
{
  QString label = ToLabel(property.getid());
  QLabel* nameLabel = new QLabel(label);
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  layout()->addWidget(nameLabel);

  QCheckBox* widget = new QCheckBox(this);
  auto result = node->getProperty(property.getid());
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setChecked(result.toBool());
  connect(widget, &QCheckBox::checkStateChanged, this, [=](Qt::CheckState state) {
    node->setProperty(property.getid(), state);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyComponentSelect(const PropertyInfo& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QLabel* nameLabel = new QLabel(ToLabel(property.getid()));
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  layout()->addWidget(nameLabel);

  QComboBox* widget = new QComboBox(this);
  for (const auto& child : mStorage->getPossibleCallers(node->id()))
  {
    auto name = child->getProperty(ConfigKeys::NAME);
    if (name.isNull() || !name.isValid())
      continue;

    widget->addItem(name.toString(), child->getid());
  }

  // Make sure the widget shows the current selected component if it exists
  auto propertyValue = node->getProperty(property.getid());
  if (propertyValue.isValid())
  {
    auto object = propertyValue.toJsonObject();
    if (object.contains(ConfigKeys::DATA))
      widget->setCurrentText(object[ConfigKeys::DATA].toString());
    else
      widget->setCurrentIndex(0);
  }
  else
  {
    widget->setCurrentText("-");
  }

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  if (property.getoptions().empty())
  {
    connect(widget, &QComboBox::currentTextChanged, this, [node, property](const QString& text) {
      UPDATE_PROPERTY(node, property.getid(), text)
      LOG_TRACE("Setting value to %s", qPrintable(text));
    });
  }
  else
  {
    for (const auto& option : property.getoptions())
    {
      if (option->gettype() == Types::PropertyTypes::EVENT_SELECT)
        LOG_WARN_ON_FAILURE(loadFieldEventSelect(widget, option->getid(), property, node, [this](const QString& nodeId, QComboBox* eventWidget) {
          eventWidget->clear();
          auto events = mStorage->getEventsFromNode(nodeId);
          for (const auto& event : events)
            eventWidget->addItem(event->getname(), event->getid());
        }));
      else if (option->gettype() == Types::PropertyTypes::TRIGGER_CALL)
        LOG_WARN_ON_FAILURE(loadFieldTriggerCall(widget, option->getid(), property, node));
      else if (option->gettype() == Types::PropertyTypes::USER_CALL)
        LOG_WARN_ON_FAILURE(loadFieldEventSelect(widget, option->getid(), property, node, [this](const QString& nodeId, QComboBox* eventWidget) {
          eventWidget->clear();
          auto events = mStorage->getEventsOfTypeFromNode(nodeId, Types::CallType::USER);
          for (const auto& event : events)
            eventWidget->addItem(event->getname(), event->getid());
        }));
      else
        LOG_WARNING("Configuration is not supported");
    }
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadFieldEventSelect(QComboBox* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node,
                                                std::function<void(const QString& nodeId, QComboBox* eventWidget)> populate)
{
  QLabel* nameEventLabel = new QLabel(ToLabel(optionId));
  nameEventLabel->setFont(Fonts::Label);

  QComboBox* eventWidget = new QComboBox(this);
  eventWidget->setObjectName(optionId);
  eventWidget->setFont(Fonts::Property);

  auto* vlayout = new QFormLayout();

  // Set starting values
  auto propertyValue = node->getProperty(property.getid());
  if (propertyValue.isValid())
  {
    // Based on the component, we can then set the current event
    auto currentComponentId = componentSelect->currentData().toString();
    populate(currentComponentId, eventWidget);

    QJsonObject object = propertyValue.toJsonObject();
    if (object[ConfigKeys::OPTIONS].toArray().size() > EVENT_INDEX)
      eventWidget->setCurrentText(object[ConfigKeys::OPTIONS][EVENT_INDEX][ConfigKeys::DATA].toString());
    else
      eventWidget->setCurrentIndex(0);

    // Finally, based on the event, we can set the arguments
    auto currentEventName = eventWidget->currentText();

    // Make sure we set the property even if nothing was selected yet
    UPDATE_PROPERTY_ARG(node, property.getid(), EVENT_INDEX, currentEventName, Types::PropertyTypes::EVENT_SELECT, false)

    LOG_WARN_ON_FAILURE(loadEventArguments(currentComponentId, currentEventName, property, node, Types::CallType::UNKNOWN, vlayout));
  }

  connect(eventWidget, &QComboBox::currentTextChanged, this, [this, node, property, componentSelect, vlayout](const QString& text) {
    if (text.isEmpty())
      return;

    clearLayout(vlayout);
    UPDATE_PROPERTY_ARG(node, property.getid(), CLEAR_INDEX, text, Types::PropertyTypes::EVENT_SELECT, false)
    LOG_WARN_ON_FAILURE(loadEventArguments(componentSelect->currentData().toString(), text, property, node, Types::CallType::UNKNOWN, vlayout));
  });

  connect(componentSelect, &QComboBox::currentTextChanged, this, [componentSelect, populate, eventWidget, node, property](const QString& text) {
    if (text.isEmpty())
      return;

    populate(componentSelect->currentData().toString(), eventWidget);
    UPDATE_PROPERTY(node, property.getid(), text)
  });

  // Add everything to the layout
  layout()->addWidget(nameEventLabel);
  layout()->addWidget(eventWidget);
  qobject_cast<QVBoxLayout*>(layout())->addLayout(vlayout);

  return VoidResult();
}

VoidResult PropertiesMenu::loadFieldTriggerCall(QComboBox* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node)
{
  auto* vlayout = new QFormLayout();
  auto currentComponentId = componentSelect->currentData().toString();
  LOG_WARN_ON_FAILURE(loadEventArguments(currentComponentId, "", property, node, Types::CallType::TRIGGER, vlayout));

  connect(componentSelect, &QComboBox::currentTextChanged, this, [this, componentSelect, vlayout, node, property](const QString& text) {
    if (text.isEmpty())
      return;

    clearLayout(vlayout);

    UPDATE_PROPERTY(node, property.getid(), text)
    LOG_WARN_ON_FAILURE(loadEventArguments(componentSelect->currentData().toString(), "", property, node, Types::CallType::TRIGGER, vlayout));
  });

  qobject_cast<QVBoxLayout*>(layout())->addLayout(vlayout);

  return VoidResult();
}

VoidResult PropertiesMenu::loadEventArguments(const QString& nodeId, const QString& flowName, const PropertyInfo& property, NodeItem* node, Types::CallType callType, QFormLayout* formLayout)
{
  std::shared_ptr<FlowSaveInfo> event = nullptr;
  if (callType == Types::CallType::UNKNOWN)
  {
    event = mStorage->getEventFromNode(nodeId, flowName);
  }
  else
  {
    auto events = mStorage->getEventsOfTypeFromNode(nodeId, callType);
    if (events.isEmpty())
      return VoidResult::Failed("Component does not have any flows");

    event = events.first();
  }

  if (!event)
    return VoidResult::Failed("Component does not have flow: " + flowName.toStdString());

  auto jsonValue = node->getProperty(property.getid());
  if (!jsonValue.isValid())
    return VoidResult::Failed("Property is not valid");

  if (event->getarguments().isEmpty())
    return VoidResult();

  LOG_TRACE("Loading %s with args: %d", qPrintable(event->getname()), event->getarguments().size());

  int index = ARG_INDEX;
  maki::WidgetAlignment alignment = {maki::WidgetAlignment::Type::FORM, formLayout};
  QJsonArray argArray = jsonValue.toJsonObject()[ConfigKeys::OPTIONS].toArray();

  for (const auto& arg : event->getarguments())
  {
    QJsonObject jsonItem = index < argArray.size() ? argArray[index].toObject() : QJsonObject();

    const auto argType = arg->gettype();
    if (argType == Types::PropertyTypes::INTEGER)
    {
      auto* field = new maki::StringWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      ADD_COMPLETER(field, node, argType);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      connect(field, &maki::StringWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = false;
        (void)value.toInt(&isLiteral);

        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::INTEGER, !isLiteral)
        LOG_DEBUG("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else if (argType == Types::PropertyTypes::REAL)
    {
      auto* field = new maki::StringWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      ADD_COMPLETER(field, node, argType);

      connect(field, &maki::StringWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = false;
        (void)value.toDouble(&isLiteral);
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::REAL, !isLiteral)
        LOG_TRACE("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else if (argType == Types::PropertyTypes::STRING)
    {
      auto* field = new maki::StringWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      ADD_COMPLETER(field, node, argType);

      connect(field, &maki::StringWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = value.size() > 2 && value.startsWith('"') && value.endsWith('"');
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::STRING, !isLiteral)
        LOG_TRACE("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else if (arg->gettype() == Types::PropertyTypes::BOOLEAN)
    {
      auto* field = new maki::StringWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      connect(field, &maki::StringWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = value == "true" || value == "false" || value == "True" || value == "False";
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::BOOLEAN, !isLiteral)
        LOG_TRACE("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else
    {
      LOG_WARNING("No support for argument of type: %s", qPrintable(Types::PropertyTypesToString(arg->gettype())));
    }

    ++index;
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyEventSelect(const PropertyInfo& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QComboBox* widget = new QComboBox(this);
  widget->setObjectName(property.getid());

  // Make sure the widget shows the current selected component if it exists
  auto currentValue = node->getProperty(property.getid());
  if (currentValue.isValid())
    widget->setCurrentText(currentValue.toString());
  else
    widget->setCurrentText("-");

  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    node->setProperty(property.getid(), text);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

QLineEdit* PropertiesMenu::loadPropertyEventArguments(const PropertyInfo& property, NodeItem* node, const QString& propertyId, const QString& eventName, QComboBox* eventWidget)
{
  QString label = ToLabel(property.getid());
  QLabel* nameLabel = new QLabel(label);

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);

  QLineEdit* widget = new QLineEdit(this);
  auto result = node->getProperty(propertyId);
  if (!result.isValid())
    return NULL;

  widget->setText(result.toString());
  connect(widget, &QLineEdit::editingFinished, this, [=]() {
    auto value = node->getProperty(propertyId);
    if (!value.isValid())
      return;

    QJsonObject object = value.toJsonObject();
    object[ConfigKeys::OPTION_DATA] = eventName;
    object["option_data_id"] = eventWidget->currentData().toString();
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);
  return widget;
}

VoidResult PropertiesMenu::onTransitionSelected(TransitionItem* transition)
{
  // Clear the frame
  clearLayout(layout());

  if (!transition)
    return VoidResult();

  mCurrentNode = transition->id();

  auto source = transition->source();
  if (source == nullptr)
    return VoidResult::Failed("Transition with no source");

  QLabel* comboLabel = new QLabel("Transition event");
  comboLabel->setFont(Fonts::Label);
  layout()->addWidget(comboLabel);

  QComboBox* eventWidget = new QComboBox(this);
  eventWidget->setPlaceholderText("-");
  eventWidget->setCurrentIndex(-1);

  // First, we add the node specific transitions
  for (const auto& t : source->configTransitions())
    eventWidget->addItem(t.event, t.event);

  // Then the generic handlers
  eventWidget->addItem("on error", "on error");
  eventWidget->addItem("on abort", "on abort");

  // Finally, the rest of the signals
  auto callers = mStorage->getPossibleCallers(source->id());
  for (const auto& caller : callers)
  {
    auto name = caller->getProperty(ConfigKeys::NAME);
    if (name.isNull() || !name.isValid())
      continue;

    auto events = mStorage->getEventsOfTypeFromNode(caller->getid(), Types::CallType::OUT);
    for (const auto& event : events)
      eventWidget->addItem(name.toString() + "." + event->getname(), "on");
  }

  // Set the initial value
  eventWidget->setCurrentText(transition->getEvent());

  connect(eventWidget, &QComboBox::currentTextChanged, this, [transition, eventWidget](const QString& text) {
    transition->setEvent(text);
    transition->setName(eventWidget->currentData().toString());
    LOG_TRACE("Setting transition to: %s and %s", qPrintable(text), qPrintable(eventWidget->currentData().toString()));
  });

  QPushButton* button = new QPushButton(this);
  button->setText(tr("Reset"));
  connect(button, &QPushButton::pressed, this, [transition, eventWidget]() {
    transition->setEvent("");
    transition->setName("");
    eventWidget->setPlaceholderText("-");
    eventWidget->setCurrentIndex(-1);
  });

  layout()->addWidget(eventWidget);
  layout()->addWidget(button);

  qobject_cast<QVBoxLayout*>(layout())->addStretch();

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
    model->setItem(newRow, 0, new QStandardItem(field->getid()));
    model->setItem(newRow, 2, new QStandardItem(Types::PropertyTypesToString(field->gettype())));

    if (field->gettype() == Types::PropertyTypes::LIST)
      model->setItem(newRow, 1, new QStandardItem(JSON::fromArray(field->getdefaultValue().toList(), ',')));
    else
      model->setItem(newRow, 1, new QStandardItem(field->getdefaultValue().toString()));
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
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  layout()->addWidget(nameLabel);

  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
  tableView->setObjectName("EventTable");
  tableView->verticalHeader()->setVisible(false);

  QStandardItemModel* model = new QStandardItemModel(0, 4);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Type"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Return"));
  model->setHorizontalHeaderItem(3, new QStandardItem("Argument"));

  tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // We do not support editing values in the table directly. I want to avoid issues caused by a wrong click
  // Instead, we open a dialog with a complete overview of the event.
  // TODO(felaze): It would be nice to also show the nodes that trigger this event
  tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView->setModel(model);

  // tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  // tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  // tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  // tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

  // tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  // tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  for (const std::shared_ptr<IFlow>& event : node->events())
  {
    // LOG_INFO("Setting events for %s (%d): %s", qPrintable(node->nodeName()), model->rowCount(), qPrintable(event->getname()));
    addEventToTable(model, model->rowCount(), std::dynamic_pointer_cast<FlowSaveInfo>(event));
  }

  // tableView->resizeRowsToContents();

  // int height = tableView->horizontalHeader()->height();
  // for (int i = 0; i < tableView->model()->rowCount(); ++i)
  //   height += tableView->rowHeight(i);

  // if (!tableView->horizontalHeader()->isHidden())
  //   height += 2;

  // tableView->setFixedHeight(height);
  // tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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
  connect(button, &QPushButton::pressed, this, [this, tableView, node, model]() {
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
  nameLabel->setObjectName("PropertyLabel");
  nameLabel->setFont(Fonts::Label);

  layout()->addWidget(nameLabel);

  // Create table to hold new fields
  QTableView* tableView = new QTableView(parent);
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
    addStateToTable(model, model->rowCount(), std::dynamic_pointer_cast<PropertyInfo>(field));

  connect(tableView, &QTableView::doubleClicked, [this, tableView, node](const QModelIndex& index) {
    openFieldDialog(tableView, node, index.row());
  });

  connect(tableView, &QTableView::customContextMenuRequested, [this, tableView, node](const QPoint& pos) {
    showContextMenu(tableView, node, pos);
  });

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, [this, tableView, node, model]() {
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

  // QAction* actionDelete = contextMenu.addAction(tr("Delete"));
  // connect(actionDelete, &QAction::triggered, this, [row, tableView, node] {
  //   auto key = static_cast<QStandardItemModel*>(tableView->model())->item(row, 0);
  //   if (key && !key->text().isNull())
  //     node->removeField(key->text());

  //   tableView->model()->removeRow(row);
  // });

  contextMenu.exec(tableView->viewport()->mapToGlobal(pos));
}

void PropertiesMenu::openEventDialog(QTableView* tableView, NodeItem* node, int row)
{
  // Open the dialog
  mCurrentDialog = new EventDialog(tr("Edit event"), this);

  auto config = row < node->events().size() ? std::dynamic_pointer_cast<FlowSaveInfo>(node->events().at(row)) : std::make_shared<FlowSaveInfo>();
  qobject_cast<EventDialog*>(mCurrentDialog)->setup(config);

  connect(mCurrentDialog, &QDialog::accepted, [this, tableView, node, row] {
    auto info = qobject_cast<EventDialog*>(mCurrentDialog)->getInfo();
    Flow* flow = node->createFlow(info->getname(), info);
    addEventToTable((QStandardItemModel*)tableView->model(), row, info);
    if (info->getmodifiable())
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

  auto config = row < node->fields().size() ? node->fields().at(row) : std::make_shared<PropertyInfo>();
  qobject_cast<FieldDialog*>(mCurrentDialog)->setup(std::dynamic_pointer_cast<PropertyInfo>(config));

  connect(mCurrentDialog, &QDialog::accepted, [this, tableView, node, row] {
    auto info = qobject_cast<FieldDialog*>(mCurrentDialog)->getInfo();
    node->setField(info->getid(), info);
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

  auto indexItem = new QStandardItem(event->getname());
  indexItem->setData(event->getid(), Qt::UserRole);
  indexItem->setData(event->getmodifiable(), Qt::UserRole + 1);

  LOG_TRACE("Adding event %s of type %s and return %s to event table",
            qPrintable(event->getname()),
            qPrintable(Types::CallTypeToString(event->gettype())),
            qPrintable(Types::PropertyTypesToString(event->getreturnType())));

  model->setItem(row, 0, indexItem);
  model->setItem(row, 1, new QStandardItem(Types::CallTypeToString(event->gettype())));
  model->setItem(row, 2, new QStandardItem(Types::PropertyTypesToString(event->getreturnType())));

  if (event->getarguments().isEmpty())
    return;

  QString args = "";
  for (auto arg : event->getarguments())
    args += arg->getid() + ", ";

  // Remove the trailing ", "
  args.chop(2);
  model->setItem(row, 3, new QStandardItem(args));
}

void PropertiesMenu::addStateToTable(QStandardItemModel* model, int row, std::shared_ptr<PropertyInfo> field)
{
  if (row >= model->rowCount())
    model->insertRow(row);

  model->setItem(row, 0, new QStandardItem(field->getid()));
  model->setItem(row, 1, new QStandardItem(Types::PropertyTypesToString(field->gettype())));

  if (field->gettype() == Types::PropertyTypes::LIST)
    model->setItem(row, 2, new QStandardItem(JSON::fromArray(field->getdefaultValue().toList(), ',')));
  else
    model->setItem(row, 2, new QStandardItem(field->getdefaultValue().toString()));
}
