#include "properties_menu.h"

#include <qobject.h>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleValidator>
#include <QGraphicsItem>
#include <QGraphicsScene>
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
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "../structure/event_dialog.h"
#include "../structure/field_dialog.h"
#include "../structure/flow_call_menu.h"
#include "../structure/svg_click_button.h"
#include "../structure/transition_event_menu.h"
#include "app_configs.h"
#include "config.h"
#include "elements/behaviour/call_capability.h"
#include "elements/behaviour/link_out_node.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "json.h"
#include "keys.h"
#include "logging.h"
#include "oclero/qlementine/Common.hpp"
#include "result.h"
#include "style_helpers.h"
#include "types.h"
#include "widgets/widget_factory.h"

static const int EVENT_INDEX = call_capability::kEventIndex;
static const int ARG_INDEX = call_capability::kArgIndex;
static const int CLEAR_INDEX = INT32_MAX;


#define UPDATE_PROPERTY(NODE, ID, VALUE)                            \
  do                                                                \
  {                                                                 \
    auto propValue = NODE->getProperty(ID);                         \
    if (!propValue.isValid())                                       \
    {                                                               \
      LOG_WARNING("Property is not valid");                         \
      return;                                                       \
    }                                                               \
                                                                    \
    QJsonObject object = propValue.toJsonObject();                  \
    const QString previous = object.value(ConfigKeys::DATA).toString(); \
    object[ConfigKeys::DATA] = VALUE;                               \
    if (previous != QString(VALUE))                                 \
      object[ConfigKeys::OPTIONS] = QJsonArray();                   \
                                                                    \
    NODE->setProperty(ID, object);                                  \
  } while (false);

#define UPDATE_PROPERTY_ARG(NODE, ID, INDEX, VALUE, DATA_TYPE, VARIABLE) \
  do                                                                     \
  {                                                                      \
    auto propValue = NODE->getProperty(ID);                              \
    if (propValue.isValid())                                             \
    {                                                                    \
      QJsonObject object = propValue.toJsonObject();                     \
      QJsonArray array = object[ConfigKeys::OPTIONS].toArray();          \
                                                                         \
      QJsonObject item;                                                  \
      item[ConfigKeys::DATA] = VALUE;                                    \
      item[ConfigKeys::TYPE] = Types::PropertyTypesToString(DATA_TYPE);  \
      item[ConfigKeys::IS_VARIABLE] = VARIABLE;                          \
      if (INDEX == EVENT_INDEX)                                          \
      {                                                                  \
        const QString previous =                                         \
            array.size() > EVENT_INDEX                                   \
                ? array.at(EVENT_INDEX).toObject().value(ConfigKeys::DATA).toString() \
                : QString();                                             \
        if (previous != QString(VALUE))                                  \
          array = QJsonArray();                                          \
      }                                                                  \
                                                                         \
      if (INDEX < array.size())                                          \
        array[INDEX] = item;                                             \
      else                                                               \
      {                                                                  \
        while (array.size() <= INDEX)                                    \
          array.append(QJsonObject());                                   \
        array[INDEX] = item;                                             \
      }                                                                  \
                                                                         \
      object[ConfigKeys::OPTIONS] = array;                               \
      NODE->setProperty(ID, object);                                     \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      LOG_WARNING("Property is not valid");                              \
    }                                                                    \
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
    {
      // Driven inside the Call capability UI (Async only when a trigger exists).
      if (call_capability::isCallNodeType(node->nodeType()) && property.id == call_capability::kModeProperty)
        continue;
      LOG_WARN_ON_FAILURE(loadPropertySelect(property, node));
    }
    else if (property.type == Types::PropertyTypes::COLOR)
    {
      if (node->isTaskContainer())
        LOG_WARN_ON_FAILURE(loadPropertyColor(property, node));
    }
    else if (property.type == Types::PropertyTypes::EVENT_SELECT)
      LOG_WARN_ON_FAILURE(loadPropertyEventSelect(property, node));
    else if (property.type == Types::PropertyTypes::COMPONENT_SELECT)
    {
      if (call_capability::isCallNodeType(node->nodeType()))
        LOG_WARN_ON_FAILURE(loadPropertyCallSelect(property, node));
      else
        LOG_WARN_ON_FAILURE(loadPropertyComponentSelect(property, node));
    }
    else if (property.type == Types::PropertyTypes::LINK_TARGET)
      LOG_WARN_ON_FAILURE(loadPropertyLinkTarget(property, node));
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

  auto widget = new maki::IntegerWidget(ToLabel(property.getid()), "", maki::WidgetAlignment::Vertical(), this);
  widget->setAcceptVariable(true);
  if (result.isValid())
    widget->setValue(result.toString());

  connect(widget, &maki::IntegerWidget::valueChanged, this, [node, property](const QString& value) {
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

  auto widget = new maki::FloatWidget(ToLabel(property.getid()), "", maki::WidgetAlignment::Vertical(), this);
  widget->setAcceptVariable(true);
  if (result.isValid())
  {
    LOG_DEBUG("Setting propoerty %s to: %.2lf", qPrintable(property.getid()), result.toDouble());
    widget->setValue(result.toDouble());
  }

  connect(widget, &maki::FloatWidget::valueChanged, this, [node, property](const QString& value) {
    if (node)
      node->setProperty(property.getid(), value);
  });

  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyColor(const PropertyInfo& property, NodeItem* node)
{
  auto* colorEditor = new maki::ColorWidget(ToLabel(property.getid()), "", maki::WidgetAlignment::Vertical(), this);
  auto result = node->getProperty(property.getid());
  if (result.isValid())
    colorEditor->setValue(QColor::fromString(result.toString()));

  connect(colorEditor, &maki::ColorWidget::valueChanged, this, [node, property](const QColor& color) {
    if (node)
      node->setProperty(property.getid(), color.name());
  });

  layout()->addWidget(colorEditor);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertySelect(const PropertyInfo& property, NodeItem* node)
{
  auto* widget = new maki::SelectorWidget(ToLabel(property.getid()), maki::WidgetAlignment::Vertical(), this);
  auto options = property.getoptions();
  for (const auto& option : options)
    widget->addItem(option->getid(), option->getid());

  auto result = node->getProperty(property.getid());
  if (result.isValid())
    widget->setValue(result.toString());

  connect(widget, &maki::SelectorWidget::valueChanged, this, [node, id = property.getid()](const QString& text) {
    if (node)
      node->setProperty(id, text);
  });

  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyLinkTarget(const PropertyInfo& property, NodeItem* node)
{
  auto* widget = new maki::SelectorWidget(ToLabel(property.getid()), maki::WidgetAlignment::Vertical(), this);
  widget->addItem(tr("(none)"), QString());

  if (auto* linkOut = dynamic_cast<LinkOutNode*>(node))
  {
    for (const auto& target : linkOut->linkInTargets())
      widget->addItem(target.name, target.id);
  }
  else if (node && node->scene())
  {
    for (QGraphicsItem* item : node->scene()->items())
    {
      if (!item || item == node || item->type() != NodeItem::Type)
        continue;

      auto* candidate = static_cast<NodeItem*>(item);
      if (candidate->nodeType() != link_nodes::kLinkInType)
        continue;

      widget->addItem(candidate->nodeName(), candidate->id());
    }
  }

  // Restore saved target id; clear the property if that Link in no longer exists.
  const QString targetId = node->getProperty(property.getid()).toString();
  widget->setData(targetId);
  if (!targetId.isEmpty() && widget->getData().toString() != targetId)
  {
    node->setProperty(property.getid(), QString());
    widget->setData(QString());
  }

  connect(widget, &maki::SelectorWidget::dataChanged, this,
          [node, id = property.getid()](const QString&, const QVariant& data) {
            if (node)
              node->setProperty(id, data.toString());
          });

  layout()->addWidget(widget);
  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyString(const PropertyInfo& property, NodeItem* node)
{
  auto* widget = new maki::StringWidget(ToLabel(property.getid()), "", maki::WidgetAlignment::Vertical(), this);
  auto result = node->getProperty(property.getid());
  if (result.isValid())
    widget->setValue(result.toString());
  else
    LOG_DEBUG("Creation of StringWidget failed: %s", qPrintable(property.getid()));

  connect(widget, &maki::StringWidget::valueChanged, this, [node, property](const QString& text) {
    if (node)
    {
      if (text.isEmpty())
        node->setProperty(property.getid(), node->nodeType().replace("Koda::", ""));
      else
        node->setProperty(property.getid(), text);
    }

    if (property.getid() == ConfigKeys::NAME)
      node->setProperty("name_auto_generated", text.isEmpty());
  });

  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyBoolean(const PropertyInfo& property, NodeItem* node)
{
  // What a hack...
  if (property.getid() == "name_auto_generated")
    return VoidResult();

  auto* widget = new maki::BooleanWidget(ToLabel(property.getid()), "", maki::WidgetAlignment::Vertical(), this);

  auto result = node->getProperty(property.getid());
  if (result.isValid())
    widget->setValue(result.toBool());
  else
    LOG_DEBUG("Creation of BooleanWidget failed: %s", qPrintable(property.getid()));

  connect(widget, &maki::BooleanWidget::valueChanged, this, [node, property](const bool value) {
    if (node)
      node->setProperty(property.getid(), value);
  });

  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyComponentSelect(const PropertyInfo& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  auto componentType = Types::PropertyTypes::EVENT_SELECT;
  if (!property.getoptions().empty())
    for (const auto& option : property.getoptions())
      if (option->gettype() == Types::PropertyTypes::FLOW_CALL ||
          option->gettype() == Types::PropertyTypes::EVENT_SELECT ||
          option->gettype() == Types::PropertyTypes::TRIGGER_CALL)
        componentType = option->gettype();

  auto* widget = new maki::SelectorWidget(ToLabel(property.getid()), maki::WidgetAlignment::Vertical(), this);
  for (const auto& child : mStorage->getPossibleCallers(node->id(), componentType))
  {
    auto name = child->getProperty(ConfigKeys::NAME);
    if (name.isNull() || !name.isValid())
      continue;

    widget->addItem(name.toString(), child->getid());
  }

  // Make sure the widget shows the current selected component if it exists
  auto selectedComponent = node->getProperty(property.getid());
  if (selectedComponent.isValid())
  {
    auto object = selectedComponent.toJsonObject();
    if (object.contains(ConfigKeys::DATA))
      widget->setValue(object[ConfigKeys::DATA].toString());
  }
  else
  {
    widget->setValue(Constants::EMPTY_COMBO);
  }

  layout()->addWidget(widget);
  if (property.getoptions().empty())
  {
    // Option-less component_select (e.g. Wait): persist name only.
    connect(widget, &maki::SelectorWidget::dataChanged, this,
            [node, property](const QString& component, const QVariant& nodeId) {
              if (!node || !nodeId.isValid())
                return;
              if (call_capability::isWaitNodeType(node->nodeType()))
                call_capability::applyCapabilitySelectionNameOnly(*node, component);
              else
                UPDATE_PROPERTY(node, property.getid(), component)
            });
  }
  else
  {
    for (const auto& option : property.getoptions())
    {
      if (option->gettype() == Types::PropertyTypes::EVENT_SELECT)
      {
        LOG_WARN_ON_FAILURE(loadFieldEventSelect(widget, option->getid(), property, node, Types::CallType::UNKNOWN, [this](const QString& nodeId, QComboBox* eventWidget) {
          eventWidget->clear();
          auto events = mStorage->getEventsOfTypeFromNode(nodeId, {Types::CallType::TRIGGER, Types::CallType::ABORT, Types::CallType::IN});
          for (const auto& event : events)
            eventWidget->addItem(event->getname(), event->getname());
        }, false));
      }
      else if (option->gettype() == Types::PropertyTypes::TRIGGER_CALL)
      {
        // This is used in async call type blocks, where the component itself has arguments
        LOG_WARN_ON_FAILURE(loadFieldTriggerCall(widget, option->getid(), property, node));
      }
      else if (option->gettype() == Types::PropertyTypes::FLOW_CALL)
      {
        LOG_WARN_ON_FAILURE(loadFieldEventSelect(widget, option->getid(), property, node, Types::CallType::USER, [this](const QString& nodeId, QComboBox* eventWidget) {
          eventWidget->clear();
          auto events = mStorage->getFlowsFromNode(nodeId);
          for (const auto& event : events)
            eventWidget->addItem(event->getname(), event->getname());
        }, true));
      }
      else
      {
        LOG_WARNING("Configuration is not supported");
      }
    }
  }
  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyCallSelect(const PropertyInfo& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  auto* capabilitySelect = new maki::SelectorWidget(ToLabel(property.getid()), maki::WidgetAlignment::Vertical(), this);
  for (const auto& child : mStorage->getPossibleCallers(node->id(), Types::PropertyTypes::EVENT_SELECT))
  {
    auto name = child->getProperty(ConfigKeys::NAME);
    if (name.isNull() || !name.isValid())
      continue;
    capabilitySelect->addItem(name.toString(), child->getid());
  }

  const auto selectedComponent = node->getProperty(property.getid());
  if (selectedComponent.isValid())
  {
    const auto object = selectedComponent.toJsonObject();
    if (object.contains(ConfigKeys::DATA))
      capabilitySelect->setValue(object[ConfigKeys::DATA].toString());
  }
  else
  {
    capabilitySelect->setValue(Constants::EMPTY_COMBO);
  }

  auto* modeSelect = new maki::SelectorWidget(tr("Call type"), maki::WidgetAlignment::Vertical(), this);
  modeSelect->addItem(tr("Sync (immediate return)"), call_capability::kModeSync);
  modeSelect->addItem(tr("Async (wait for return)"), call_capability::kModeAsync);

  auto* eventCombo = new QComboBox(this);
  auto* eventSelect = new maki::SelectorWidget(tr("Event"), eventCombo, maki::WidgetAlignment::Vertical(), this);
  auto* argsGroup = new maki::WidgetGroup(tr("Arguments"), oclero::qlementine::TextRole::Default, this);

  layout()->addWidget(capabilitySelect);
  layout()->addWidget(modeSelect);
  layout()->addWidget(eventSelect);
  layout()->addWidget(argsGroup);

  auto populateEventsForMode = [this](const QString& capabilityId, const QString& mode, QComboBox* combo) {
    combo->clear();
    if (capabilityId.isEmpty())
      return;

    const auto events = mStorage->getEventsOfTypeFromNode(capabilityId, call_capability::eventTypesForMode(mode));
    for (const auto& event : events)
      combo->addItem(event->getname(), event->getname());
  };

  auto refreshCallDetails = [this, node, property, capabilitySelect, modeSelect, eventSelect, eventCombo, argsGroup,
                             populateEventsForMode]() {
    const QString capabilityId = capabilitySelect->getData().toString();
    const QString capabilityName = capabilitySelect->getValue();

    clearLayout(argsGroup->layout());
    modeSelect->hide();
    eventSelect->hide();
    argsGroup->hide();

    if (capabilityId.isEmpty())
      return;

    const bool canAsync = call_capability::canAsync(*mStorage, capabilityId);
    const bool canSync = call_capability::canSync(*mStorage, capabilityId);
    if (!canSync && !canAsync)
      return;

    QString mode = node->getProperty(call_capability::kModeProperty).toString();
    QString resolvedEvent = call_capability::currentEventName(*node);
    call_capability::resolveModeAndEvent(*mStorage, capabilityId, mode, resolvedEvent);
    if (mode.isEmpty())
      return;

    node->setProperty(call_capability::kModeProperty, mode);

    // Rebuild mode items: Async only when a trigger exists.
    modeSelect->blockSignals(true);
    if (auto* combo = modeSelect->findChild<QComboBox*>())
      combo->clear();
    modeSelect->addItem(tr("Sync (immediate return)"), call_capability::kModeSync);
    if (canAsync)
      modeSelect->addItem(tr("Async (wait for return)"), call_capability::kModeAsync);
    modeSelect->setData(mode);
    modeSelect->blockSignals(false);
    modeSelect->show();

    populateEventsForMode(capabilityId, mode, eventCombo);
    eventSelect->show();

    QString eventName = resolvedEvent;
    if (!eventName.isEmpty() && eventCombo->findData(eventName) < 0)
      eventName.clear();
    if (eventName.isEmpty() && eventCombo->count() > 0)
      eventName = eventCombo->itemData(0).toString();

    if (eventName.isEmpty())
      return;

    eventSelect->blockSignals(true);
    eventSelect->setValue(eventName);
    eventSelect->blockSignals(false);
    call_capability::applyModeAndEvent(*node, mode, eventName);
    LOG_WARN_ON_FAILURE(loadEventArguments(capabilityId, eventName, property, node, Types::CallType::UNKNOWN, argsGroup));
    updateBlockName(node, capabilityName, eventName);
  };

  refreshCallDetails();

  connect(capabilitySelect, &maki::SelectorWidget::dataChanged, this,
          [this, node, property, refreshCallDetails](const QString& component, const QVariant& nodeId) {
            if (!nodeId.isValid())
              return;
            call_capability::applyCapabilitySelection(*node, component, nodeId.toString(), mStorage.get());
            // Keep the selector's data in sync; applyCapabilitySelection already wrote capability + mode/event.
            Q_UNUSED(property);
            refreshCallDetails();
          });

  connect(modeSelect, &maki::SelectorWidget::dataChanged, this,
          [this, node, refreshCallDetails](const QString&, const QVariant& modeData) {
            const QString mode = modeData.toString();
            if (mode != call_capability::kModeSync && mode != call_capability::kModeAsync)
              return;
            node->setProperty(call_capability::kModeProperty, mode);
            refreshCallDetails();
          });

  connect(eventSelect, &maki::SelectorWidget::valueChanged, this,
          [this, node, property, capabilitySelect, argsGroup](const QString& eventName) {
            if (eventName.isEmpty())
              return;
            clearLayout(argsGroup->layout());
            UPDATE_PROPERTY_ARG(node, property.getid(), EVENT_INDEX, eventName, Types::PropertyTypes::EVENT_SELECT, false)
            LOG_WARN_ON_FAILURE(loadEventArguments(capabilitySelect->getData().toString(), eventName, property, node, Types::CallType::UNKNOWN, argsGroup));
            updateBlockName(node, capabilitySelect->getValue(), eventName);
          });

  return VoidResult();
}

VoidResult PropertiesMenu::loadFieldEventSelect(maki::SelectorWidget* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node, Types::CallType callType,
                                                std::function<void(const QString& nodeId, QComboBox* eventWidget)> populate, bool allowCreateFlow)
{
  QComboBox* eventCombo = new QComboBox(this);
  auto* widget = new maki::SelectorWidget(ToLabel(optionId), eventCombo, maki::WidgetAlignment::Vertical(), this);
  auto* group = new maki::WidgetGroup(tr("Arguments"), oclero::qlementine::TextRole::Default, this);

  // Set starting values
  auto propertyValue = node->getProperty(property.getid());
  if (propertyValue.isValid())
  {
    // Based on the component, we can then set the current event
    auto currentComponentId = componentSelect->getData().toString();
    populate(currentComponentId, eventCombo);

    QJsonObject object = propertyValue.toJsonObject();
    if (object.contains(ConfigKeys::OPTIONS) && object[ConfigKeys::OPTIONS].toArray().size() > EVENT_INDEX)
    {
      auto currentEvent = object[ConfigKeys::OPTIONS][EVENT_INDEX][ConfigKeys::DATA].toString();
      widget->setValue(currentEvent);

      // Finally, based on the event, we can set the arguments
      LOG_WARN_ON_FAILURE(loadEventArguments(currentComponentId, currentEvent, property, node, callType, group));
    }
    else
    {
      group->hide();
    }
  }

  // When the event changes
  connect(widget, &maki::SelectorWidget::valueChanged, this, [this, node, property, componentSelect, group, callType](const QString& eventName) {
    if (eventName.isEmpty())
      return;

    clearLayout(group->layout());
    UPDATE_PROPERTY_ARG(node, property.getid(), EVENT_INDEX, eventName, Types::PropertyTypes::EVENT_SELECT, false)
    LOG_WARN_ON_FAILURE(loadEventArguments(componentSelect->getData().toString(), eventName, property, node, callType, group));

    // Update the node name
    updateBlockName(node, componentSelect->getValue(), eventName);
  });

  // When the component itself changes
  connect(componentSelect, &maki::SelectorWidget::dataChanged, this, [this, populate, eventCombo, node, id = property.getid()](const QString& component, const QVariant& nodeId) {
    if (!nodeId.isValid())
      return;

    populate(nodeId.toString(), eventCombo);
    UPDATE_PROPERTY(node, id, component)
    updateBlockName(node, component, "");
  });

  // Add everything to the layout
  if (allowCreateFlow)
  {
    auto* flowRow = new QHBoxLayout();
    flowRow->setContentsMargins(0, 0, 0, 0);
    flowRow->setSpacing(4);
    flowRow->addWidget(widget, 1);

    auto* createFlowButton = new SvgClickButton(iconPathFromTheme(QStringLiteral("button_addflow.svg")), QSize(16, 16), this);
    createFlowButton->setToolTip(tr("Create flow"));
    createFlowButton->setEnabled(!componentSelect->getData().toString().isEmpty());
    flowRow->addWidget(createFlowButton, 0, Qt::AlignTop);

    connect(createFlowButton, &SvgClickButton::clicked, this, [this, componentSelect, populate, eventCombo, widget, node, property, group, callType]() {
      const QString taskId = componentSelect->getData().toString();
      if (taskId.isEmpty())
        return;

      const auto info = FlowCallMenu::promptNewFlow(this);
      if (!info)
        return;

      const QString flowName = info->getname();
      populate(taskId, eventCombo);
      widget->addItem(flowName, flowName);
      widget->setValue(flowName);

      clearLayout(group->layout());
      UPDATE_PROPERTY_ARG(node, property.getid(), EVENT_INDEX, flowName, Types::PropertyTypes::EVENT_SELECT, false)
      LOG_WARN_ON_FAILURE(loadEventArguments(taskId, flowName, property, node, callType, group));
      updateBlockName(node, componentSelect->getValue(), flowName);
      emit createFlow(taskId, info);
    });

    connect(componentSelect, &maki::SelectorWidget::dataChanged, createFlowButton, [createFlowButton](const QString&, const QVariant& nodeId) {
      createFlowButton->setEnabled(nodeId.isValid() && !nodeId.toString().isEmpty());
    });

    auto* flowRowWidget = new QWidget(this);
    flowRowWidget->setLayout(flowRow);
    layout()->addWidget(flowRowWidget);
  }
  else
  {
    layout()->addWidget(widget);
  }
  layout()->addWidget(group);

  return VoidResult();
}

VoidResult PropertiesMenu::loadFieldTriggerCall(maki::SelectorWidget* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node)
{
  auto* group = new maki::WidgetGroup(tr("Arguments"), oclero::qlementine::TextRole::Default, this);
  auto currentComponentId = componentSelect->getData().toString();
  LOG_WARN_ON_FAILURE(loadEventArguments(currentComponentId, "", property, node, Types::CallType::TRIGGER, group));

  connect(componentSelect, &maki::SelectorWidget::dataChanged, this, [this, currentComponentId, group, node, property](const QString& component, const QVariant& nodeId) {
    if (!nodeId.isValid())
      return;

    clearLayout(group->layout());

    UPDATE_PROPERTY(node, property.getid(), component)
    LOG_WARN_ON_FAILURE(loadEventArguments(nodeId.toString(), "", property, node, Types::CallType::TRIGGER, group));
    updateBlockName(node, component, "");
  });

  layout()->addWidget(group);

  return VoidResult();
}

VoidResult PropertiesMenu::loadEventArguments(const QString& nodeId, const QString& flowName, const PropertyInfo& property, NodeItem* node, Types::CallType callType, maki::WidgetGroup* group)
{
  std::shared_ptr<FlowSaveInfo> event = nullptr;
  if (callType == Types::CallType::USER)
  {
    event = mStorage->getFlowFromNode(nodeId, flowName);
  }
  else if (!flowName.isEmpty())
  {
    event = mStorage->getEventFromNode(nodeId, flowName);
  }
  else if (callType == Types::CallType::UNKNOWN)
  {
    auto events = mStorage->getEventsFromNode(nodeId);
    if (!events.isEmpty())
      event = events.first();
  }
  else
  {
    auto events = mStorage->getEventsOfTypeFromNode(nodeId, {callType});
    if (!events.isEmpty())
      event = events.first();
  }

  if (!event)
  {
    group->hide();
    return VoidResult::Failed("Component does not have flow: " + flowName.toStdString());
  }

  auto jsonValue = node->getProperty(property.getid());
  if (!jsonValue.isValid())
  {
    group->hide();
    return VoidResult::Failed("Property is not valid");
  }

  if (event->getarguments().isEmpty())
  {
    group->hide();
    return VoidResult();
  }

  LOG_DEBUG("Loading event %s with args: %d", qPrintable(event->getname()), event->getarguments().size());

  group->show();
  int index = ARG_INDEX;
  maki::WidgetAlignment alignment = {
      .type = maki::WidgetAlignment::Type::FORM,
      .direction = maki::WidgetAlignment::Direction::SPREAD,
      .group = group,
      .labelWidth = 75,
  };

  QJsonArray argArray = jsonValue.toJsonObject()[ConfigKeys::OPTIONS].toArray();
  for (const auto& arg : event->getarguments())
  {
    QJsonObject jsonItem = index < argArray.size() ? argArray[index].toObject() : QJsonObject();
    const auto argType = arg->gettype();
    if (argType == Types::PropertyTypes::INTEGER)
    {
      auto* field = new maki::IntegerWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      field->setAcceptVariable(true);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      addCompleter(field->widget(), node->id(), argType);
      connect(field, &maki::IntegerWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = false;
        (void)value.toInt(&isLiteral);
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::INTEGER, !isLiteral)
        // LOG_DEBUG("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else if (argType == Types::PropertyTypes::REAL)
    {
      auto* field = new maki::FloatWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      field->setAcceptVariable(true);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      addCompleter(field->widget(), node->id(), argType);
      connect(field, &maki::FloatWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = false;
        (void)value.toDouble(&isLiteral);
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::REAL, !isLiteral)
        // LOG_DEBUG("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else if (argType == Types::PropertyTypes::STRING)
    {
      auto* field = new maki::StringWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      addCompleter(field->widget(), node->id(), argType);
      connect(field, &maki::StringWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = value.size() > 2 && value.startsWith('"') && value.endsWith('"');
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::STRING, !isLiteral)
        // LOG_DEBUG("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else if (argType == Types::PropertyTypes::BOOLEAN)
    {
      auto* field = new maki::StringWidget(arg->getid(), arg->getdefaultValue().toString(), alignment, this);
      if (jsonItem.contains(ConfigKeys::DATA))
        field->setValue(jsonItem[ConfigKeys::DATA].toString());

      addCompleter(field->widget(), node->id(), argType, {"true", "false"});
      connect(field, &maki::StringWidget::valueChanged, this, [property, node, index](const QString& value) {
        bool isLiteral = value == "true" || value == "false" || value == "True" || value == "False";
        UPDATE_PROPERTY_ARG(node, property.getid(), index, value, Types::PropertyTypes::BOOLEAN, !isLiteral)
        // LOG_DEBUG("Set property %s argument (%d) to %s", qPrintable(property.getid()), index, qPrintable(value));
      });
    }
    else
    {
      LOG_WARNING("No support for argument of type: %s", qPrintable(Types::PropertyTypesToString(argType)));
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

void PropertiesMenu::addCompleter(oclero::qlementine::LineEdit* field, const QString& nodeId, const Types::PropertyTypes dataType, QStringList variables)
{
  auto parentStates = mStorage->getPossibleStates(nodeId);
  for (const auto& state : parentStates)
  {
    if (!state)
      continue;

    if (state->gettype() == dataType)
      variables.append(state->getid());
  }

  auto* completer = new QCompleter(variables, field);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  completer->setCaseSensitivity(Qt::CaseInsensitive);

  field->setCompleter(completer);
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

  // SubflowBlock loop begin/end transitions have no configurable event.
  // Abort/error transitions are created from ports and are not menu-editable.
  if (source->isSubflowContainer() || (transition->destination() && transition->destination()->isSubflowContainer())
      || transition->isPortBoundEvent())
  {
    qobject_cast<QVBoxLayout*>(layout())->addStretch();
    return VoidResult();
  }

  auto* eventWidget = new maki::SelectorWidget(tr("Transition event"), maki::WidgetAlignment::Vertical(), this);
  for (const auto& option : TransitionEventMenu::buildOptions(source, mStorage.get()))
    eventWidget->addItem(option.first, option.second);

  // Set the initial value
  auto currentEvent = transition->getEvent();
  eventWidget->setValue(currentEvent.isEmpty() ? Constants::EMPTY_COMBO : currentEvent);

  connect(eventWidget, &maki::SelectorWidget::dataChanged, this, [transition](const QString& text, const QVariant& data) {
    // TransitionItem::setEvent normalizes EMPTY_COMBO ("-") to an empty label.
    transition->setEvent(text);
    transition->setName(data.toString());
    LOG_TRACE("Setting transition to: %s and %s", qPrintable(transition->getEvent()), qPrintable(data.toString()));
  });

  QPushButton* button = new QPushButton(this);
  button->setText(tr("Reset"));
  connect(button, &QPushButton::pressed, this, [transition, eventWidget]() {
    transition->setEvent("");
    transition->setName("");
    eventWidget->setValue(Constants::EMPTY_COMBO);
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
    if (!flow)
      return;
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

void PropertiesMenu::updateBlockName(NodeItem* node, const QString& componentName, const QString& eventName) const
{
  if (!node)
    return;

  // Call / Wait auto-titles are owned by the node classes themselves.
  if (call_capability::isCallNodeType(node->nodeType()) || call_capability::isWaitNodeType(node->nodeType()))
    return;

  // Sanity check, make sure the name property exists
  auto name = node->getProperty(ConfigKeys::NAME);
  if (!name.isValid())
    return;

  // We are only allowed to update the name if it wasn't set by the user
  auto wasAutoGenerated = node->getProperty("name_auto_generated");
  if (wasAutoGenerated.isValid() && !wasAutoGenerated.toBool())
    return;

  // Flow call keeps a fixed outer label; task/flow are shown on the body / popup.
  if (node->nodeType() == QStringLiteral("Koda::Flow call"))
  {
    node->setProperty(ConfigKeys::NAME, QStringLiteral("Flow Call"));
    return;
  }

  auto newName = componentName + (eventName.isEmpty() ? "" : " " + eventName);
  node->setProperty(ConfigKeys::NAME, newName);
}
