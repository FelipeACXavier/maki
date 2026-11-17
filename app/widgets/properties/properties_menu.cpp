#include "properties_menu.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QPushButton>
#include <QScopedValueRollback>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/Label.hpp>

#include "../dialogs/prompt.h"
#include "../scroll_area.h"
#include "../structure/event_dialog.h"
#include "app_configs.h"
#include "config_table.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "keys.h"
#include "logging.h"
#include "macros.h"
#include "mission_parameter.h"
#include "oclero/qlementine/Common.hpp"
#include "property_info.h"
#include "style_helpers.h"

namespace
{
Types::ControlTypes callControl(const maki::RecordValue& record)
{
  if (recordHasField(record, ConfigKeys::FLOW))
    return Types::ControlTypes::FLOW_CALL;

  if (recordHasField(record, ConfigKeys::EVENT))
    return Types::ControlTypes::EVENT_SELECT;

  return Types::ControlTypes::TRIGGER_CALL;
}
}  // namespace

#define BLOCK_AND_SET(EDITOR, VALUE)      \
  do                                      \
  {                                       \
    const QSignalBlocker blocker(EDITOR); \
    EDITOR->setValue(VALUE);              \
  } while (false);

PropertiesMenu::PropertiesMenu(QWidget* parent)
    : QFrame(parent)
    , mCurrentNode("")
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();

  auto* rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(theme.spacing, theme.spacing, 0, 0);
  rootLayout->setSpacing(theme.spacing);

  auto* scrollArea = new StyledScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setBackgroundRole(StyledScrollArea::BackgroundRole::Base);

  // Wrapper fills the scroll area's viewport.
  auto* scrollContents = new QWidget(scrollArea);
  scrollContents->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

  auto* scrollLayout = new QVBoxLayout(scrollContents);
  scrollLayout->setContentsMargins(0, 0, 0, 0);
  scrollLayout->setSpacing(0);

  mFrame = new StyledFrame(this);
  mFrame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  mFrame->setBorderRole(StyledFrame::BorderRole::Mid);
  mFrame->setRadius(theme.borderRadius);
  mFrame->setBorderWidth(theme.borderWidth);
  mFrame->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

  auto* frameLayout = new QVBoxLayout(mFrame);
  frameLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  frameLayout->setAlignment(Qt::AlignTop);
  frameLayout->setSpacing(theme.spacing);

  scrollLayout->addWidget(mFrame);
  scrollArea->setWidget(scrollContents);

  rootLayout->addWidget(scrollArea);
}

QLayout* PropertiesMenu::layout() const
{
  return mFrame ? mFrame->layout() : QFrame::layout();
}

VoidResult PropertiesMenu::start()
{
  mDocumentSubscription = SaveInfo::instance().subscribe([this](const DocumentEvent& event) {
    switch (event.type)
    {
      case DocumentEvent::Type::NodeModified:
      {
        const auto* data = std::get_if<NodeEvent>(&event.data);
        if (!data)
          return;

        const auto nodeId = data->id;
        const auto propertyId = data->propertyId;
        if (nodeId != mCurrentNode)
          return;

        LOG_DEBUG("Property {} of current node changed: {}", propertyId, mCurrentNode);

        const auto node = SaveInfo::instance().getNodeWithId(nodeId);
        if (!node)
          return;

        // For compound values, it is easier to just rebuild the menu
        if (data->propertyValue.isRecord())
          onNodeModified(nodeId);
        else if (auto* editor = findChild<maki::InputWidget*>(propertyId))
          BLOCK_AND_SET(editor, data->propertyValue);

        break;
      }
      case DocumentEvent::Type::FlowAdded:
      {
        const auto* data = std::get_if<FlowEvent>(&event.data);
        if (!data)
          return;

        onFlowAdded(data->id, data->ownerNodeId);
      }
      case DocumentEvent::Type::FlowRemoved:
      {
        const auto* data = std::get_if<FlowEvent>(&event.data);
        if (!data)
          return;

        onFlowRemoved(data->id, data->ownerNodeId);
      }
      default:
        break;
    }
  });

  return VoidResult();
}

VoidResult PropertiesMenu::onNodeRemoved(const QString& nodeId)
{
  if (nodeId != mCurrentNode)
    return VoidResult();

  clearLayout(layout());
  mCurrentNode.clear();
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeModified(const QString& nodeId)
{
  clearLayout(layout());

  // Make sure no events are sent on creation
  LOG_WARN_ON_FAILURE(loadProperties(nodeId));
  LOG_WARN_ON_FAILURE(loadControls(nodeId));

  if (auto* vLayout = qobject_cast<QVBoxLayout*>(layout()))
    vLayout->addStretch();

  return VoidResult();
}

VoidResult PropertiesMenu::onNodeSelected(const QString& nodeId, bool selected)
{
  // If we are already showing this node, then there is nothing to do
  if (!nodeId.isEmpty() && nodeId == mCurrentNode)
    return VoidResult();

  if (nodeId.isEmpty() || !selected)
    return onNodeRemoved(mCurrentNode);

  mCurrentNode = nodeId;
  emit nodeFocused(mCurrentNode);

  return onNodeModified(nodeId);
}

VoidResult PropertiesMenu::onFlowAdded(const QString& /* flowId */, const QString& nodeId)
{
  if (nodeId != mCurrentNode)
    return VoidResult();

  return onNodeModified(nodeId);
}

VoidResult PropertiesMenu::onFlowRemoved(const QString& /* flowId */, const QString& nodeId)
{
  if (nodeId != mCurrentNode)
    return VoidResult();

  return onNodeModified(nodeId);
}

VoidResult PropertiesMenu::loadProperties(const QString& nodeId)
{
  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return VoidResult::Failed("Cannot load properties for a null node");

  const auto nodeConfig = ConfigurationTable::instance().get(nodeInfo->getnodeId());
  if (!nodeInfo)
    return VoidResult::Failed("Could not create properties for unknown node with id: {}", nodeId);

  for (const auto& property : nodeInfo->getproperties())
  {
    if (!property)
      continue;

    LOG_DEBUG("Loading property: {} {} {}", property->getname(), maki::asValue(property->getvalue())->toReadable(), (int)property->getcontrol());
    switch (property->getcontrol())
    {
      case Types::ControlTypes::SELECT:
      case Types::ControlTypes::EVENT_SELECT:
        LOG_WARN_ON_FAILURE(loadSelectProperty(property, nodeId));
        break;

      case Types::ControlTypes::LINK:
        LOG_WARN_ON_FAILURE(loadLinkProperty(property, nodeId));
        break;

      case Types::ControlTypes::COLOR:
        LOG_WARN_ON_FAILURE(loadColorProperty(property, nodeId));
        break;

      case Types::ControlTypes::COMPONENT_SELECT:
        LOG_WARN_ON_FAILURE(loadComponentSelectProperty(property, nodeId));
        break;

      default:
        LOG_WARN_ON_FAILURE(loadValueProperty(property, nodeId));
        break;
    }
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadValueProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId)
{
  if (!property || nodeId.isEmpty())
    return VoidResult::Failed("Cannot create property editor without a property and node");

  const auto* storedValue = maki::asValue(property->getvalue());
  if (!storedValue)
    return VoidResult::Failed("Property '{}' does not contain a MAKI Value", property->getid().toStdString());

  // Void is used for special types and should not be added here
  bool isBoolean = false;
  if (property->gettype().isPrimitive())
  {
    if (property->gettype().primitiveKind() == koda::types::PrimitiveKind::Void)
      return VoidResult();
    else if (property->gettype().primitiveKind() == koda::types::PrimitiveKind::Bool)
      isBoolean = true;
  }

  auto* editor = maki::ValueEditorFactory::create(ToLabel(property->getid()), property->gettype(), *storedValue,
                                                  isBoolean ? maki::WidgetAlignment::Inline() : maki::WidgetAlignment::Vertical(), this);
  if (!editor)
    return VoidResult::Failed("Could not create editor for property '{}' of type '{}'", property->getid(), property->gettype().toString());

  editor->setObjectName(property->getid());
  addCompleter(editor, nodeId, property->gettype());

  connect(editor, &maki::InputWidget::valueChanged, this, [this, nodeId, editor, id = property->getid()] {
    auto value = editor->getValue();
    const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
    if (!nodeInfo)
      return;

    // Preserve the old automatic-name behaviour, but use Value throughout.
    if (id == ConfigKeys::NAME)
    {
      const auto text = value.toString();
      const bool automaticallyGenerated = text.isEmpty();

      if (automaticallyGenerated)
        value = maki::Value::createString(nodeInfo->getnodeId().replace(ConfigKeys::KODA_NAMESPACE, ""));

      if (nodeInfo->getProperty("name_auto_generated"))
        nodeInfo->setProperty("name_auto_generated", maki::Value::createBool(automaticallyGenerated));
    }

    nodeInfo->setProperty(id, value);
    emit propertyUpdated(id, nodeId, value);
  });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadColorProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId)
{
  if (!property || nodeId.isEmpty())
    return VoidResult::Failed("Cannot create color editor without a property and node");

  const auto value = maki::parameterValue(property.get());
  auto* editor = new maki::ColorWidget(ToLabel(property->getid()), "", maki::WidgetAlignment::Vertical(), this);
  editor->setObjectName(property->getid());

  if (value.kind() == IValue::Kind::Color)
    BLOCK_AND_SET(editor, std::get<QColor>(value.data))
  else if (value.isValid())
    BLOCK_AND_SET(editor, QColor::fromString(value.toString()))

  connect(editor, &maki::ColorWidget::valueChanged, this, [this, nodeId, id = property->getid()](const QColor& color) {
    const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
    if (!nodeInfo)
      return;

    maki::Value value;
    value.data = color;
    nodeInfo->setProperty(id, value);

    emit propertyUpdated(id, nodeId, value);
  });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadSelectProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId)
{
  if (!property || nodeId.isEmpty())
    return VoidResult::Failed("Cannot create selector without a property and node");

  auto record = maki::parameterRecord(property.get());
  if (!recordHasField(record, ConfigKeys::VALUE) || !recordHasField(record, ConfigKeys::ITEMS))
    return VoidResult::Failed("Select property '{}' must be a record containing 'value' and 'items'", property->getid().toStdString());

  const auto items = maki::recordList(record, ConfigKeys::ITEMS);
  const auto selected = record.at(ConfigKeys::VALUE.toStdString());

  auto* editor = new maki::SelectorWidget(ToLabel(property->getid()), maki::WidgetAlignment::Vertical(), this);
  editor->setObjectName(property->getid());
  for (size_t i = 0; i < items.size(); ++i)
    editor->addItem(items[i].toString(), static_cast<qulonglong>(i));

  BLOCK_AND_SET(editor, selected.toString());
  connect(editor, &maki::SelectorWidget::dataChanged, this,
          [nodeId, id = property->getid(), record, items](const QString& /* text */, const QVariant& data) mutable {
            if (!data.isValid())
              return;

            const auto index = data.toULongLong();
            if (index >= items.size())
              return;

            if (const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId))
            {
              record[ConfigKeys::VALUE.toStdString()] = items[index];
              nodeInfo->setProperty(id, maki::Value::createRecord(record));
            }
          });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadLinkProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId)
{
  if (!property || nodeId.isEmpty())
    return VoidResult::Failed("Cannot create link control without a property and node");

  auto record = maki::parameterRecord(property.get());
  if (!recordHasField(record, ConfigKeys::COMPONENT) || !recordHasField(record, ConfigKeys::ID))
    return VoidResult::Failed("Select property '{}' must be a record containing 'component' and 'id'", property->getid().toStdString());

  const auto component = maki::recordString(record, ConfigKeys::COMPONENT);
  auto* editor = new maki::SelectorWidget(ToLabel(property->getid()), maki::WidgetAlignment::Vertical(), this);
  editor->setObjectName(property->getid());

  const auto targetNodes = SaveInfo::instance().getLinkNodes();
  for (int i = 0; i < targetNodes.size(); ++i)
    editor->addItem(targetNodes[i].second, targetNodes[i].first);  // Add name and id of node

  editor->addItem(Constants::EMPTY_COMBO, Constants::EMPTY_COMBO);
  BLOCK_AND_SET(editor, component)

  connect(editor, &maki::SelectorWidget::dataChanged, this,
          [nodeId, id = property->getid(), record, targetNodes](const QString& text, const QVariant& data) mutable {
            if (!data.isValid())
              return;

            if (text == Constants::EMPTY_COMBO)
            {
              record[ConfigKeys::COMPONENT.toStdString()] = maki::Value::createString("");
              record[ConfigKeys::ID.toStdString()] = maki::Value::createString("");
            }
            else
            {
              record[ConfigKeys::COMPONENT.toStdString()] = maki::Value::createString(text);
              record[ConfigKeys::ID.toStdString()] = maki::Value::createString(data.toString());
            }

            if (const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId))
              nodeInfo->setProperty(id, maki::Value::createRecord(record));
          });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadComponentSelectProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId)
{
  if (!property || nodeId.isEmpty())
    return VoidResult::Failed("Cannot create component selector without a property and node");

  auto record = maki::parameterRecord(property.get());
  if (!recordHasField(record, ConfigKeys::COMPONENT))
    return VoidResult::Failed("Component-select property '{}' must contain a 'component' field", property->getid().toStdString());

  const auto mode = callControl(record);
  auto* componentEditor = new maki::SelectorWidget(ToLabel(property->getid()), maki::WidgetAlignment::Vertical(), this);
  componentEditor->setObjectName(property->getid());

  const auto candidates = SaveInfo::instance().getPossibleCallers(nodeId, mode);
  for (const auto& candidate : candidates)
  {
    if (!candidate)
      continue;

    const auto* name = candidate->getProperty(ConfigKeys::NAME);
    if (!name || !name->getvalue() || !name->getvalue()->isValid())
      continue;

    componentEditor->addItem(name->getvalue()->toStringValue(), QStringList{candidate->getid(), candidate->getnodeId()});
  }

  BLOCK_AND_SET(componentEditor, maki::recordString(record, ConfigKeys::COMPONENT))
  layout()->addWidget(componentEditor);

  auto* callEditor = static_cast<maki::SelectorWidget*>(nullptr);
  auto* argumentsGroup = new maki::WidgetGroup(tr("Arguments"), oclero::qlementine::TextRole::Default, this);

  const bool hasNamedCall = mode == Types::ControlTypes::EVENT_SELECT || mode == Types::ControlTypes::FLOW_CALL;
  if (hasNamedCall)
  {
    const QString label = mode == Types::ControlTypes::FLOW_CALL ? tr("Flow") : tr("Event");
    callEditor = new maki::SelectorWidget(label, maki::WidgetAlignment::Vertical(), this);
    callEditor->setObjectName(property->getid() + "::event");
    layout()->addWidget(callEditor);
  }

  layout()->addWidget(argumentsGroup);

  auto findCall = [mode](const QString& componentId, const QString& callName) -> std::shared_ptr<FlowSaveInfo> {
    if (componentId.isEmpty())
      return nullptr;

    if (mode == Types::ControlTypes::FLOW_CALL)
      return SaveInfo::instance().getFlowFromNode(componentId, callName);

    if (mode == Types::ControlTypes::EVENT_SELECT)
      return SaveInfo::instance().getEventFromNode(componentId, callName);

    const auto events = SaveInfo::instance().getEventsOfTypeFromNode(componentId, {Types::CallType::TRIGGER});
    return events.isEmpty() ? nullptr : events.first();
  };
  auto populateCalls = [mode, callEditor](const QString& componentId) {
    if (!callEditor)
      return;

    callEditor->widget()->clear();

    if (mode == Types::ControlTypes::FLOW_CALL)
    {
      for (const auto& flow : SaveInfo::instance().getFlowsFromNode(componentId))
        if (flow)
          callEditor->addItem(flow->getname(), flow->getid());
    }
    else
    {
      for (const auto& event : SaveInfo::instance().getEventsOfTypeFromNode(componentId, {Types::CallType::TRIGGER, Types::CallType::ABORT}))
        if (event)
          callEditor->addItem(event->getname(), event->getid());
    }
  };

  // Initialise dependent fields before connecting change handlers.
  const auto currentComponentId = componentEditor->getData().toStringList().isEmpty() ? Constants::EMPTY_COMBO
                                                                                      : componentEditor->getData().toStringList().at(0);
  if (callEditor)
  {
    populateCalls(currentComponentId);
    BLOCK_AND_SET(callEditor, maki::recordString(record, mode == Types::ControlTypes::FLOW_CALL ? ConfigKeys::FLOW : ConfigKeys::EVENT))
  }

  auto currentCall = findCall(currentComponentId, mode == Types::ControlTypes::FLOW_CALL      ? maki::recordString(record, ConfigKeys::FLOW)
                                                  : mode == Types::ControlTypes::EVENT_SELECT ? maki::recordString(record, ConfigKeys::EVENT)
                                                                                              : QString());
  LOG_WARN_ON_FAILURE(loadCallArguments(currentCall, property->getid(), nodeId, argumentsGroup));

  if (callEditor)
  {
    connect(callEditor, &maki::SelectorWidget::dataChanged, this,
            [this, nodeId, propertyId = property->getid(), componentEditor, argumentsGroup, findCall, mode](const QString& callName, const QVariant& callId) {
              if (callName.isEmpty())
                return;

              if (const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId))
              {
                const auto* current = nodeInfo->getProperty(propertyId);
                auto valueRecord = maki::parameterRecord(current);
                const auto key = mode == Types::ControlTypes::FLOW_CALL ? ConfigKeys::FLOW : ConfigKeys::EVENT;
                setRecordValue(valueRecord, key, maki::Value::createString(callName));
                setRecordValue(valueRecord, ConfigKeys::CALL_ID, maki::Value::createString(callId.toString()));
                setRecordValue(valueRecord, ConfigKeys::ARGUMENTS, maki::Value::createList({}));
                nodeInfo->setProperty(propertyId, maki::Value::createRecord(valueRecord));
                argumentsGroup->clear();
                const auto call = findCall(componentEditor->getData().toString(), callName);
                LOG_WARN_ON_FAILURE(loadCallArguments(call, propertyId, nodeId, argumentsGroup));

                updateBlockName(nodeInfo, componentEditor->getValue(), callName);
              }
            });
  }

  connect(componentEditor, &maki::SelectorWidget::dataChanged, this,
          [this, nodeId, propertyId = property->getid(), callEditor, argumentsGroup, populateCalls, findCall, mode](const QString& componentName,
                                                                                                                    const QVariant& componentInfo) {
            if (!componentInfo.isValid() || componentInfo.toStringList().size() != 2)
              return;

            if (const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId))
            {
              const auto* current = nodeInfo->getProperty(propertyId);
              auto valueRecord = maki::parameterRecord(current);
              setRecordValue(valueRecord, ConfigKeys::COMPONENT, maki::Value::createString(componentName));
              setRecordValue(valueRecord, ConfigKeys::ARGUMENTS, maki::Value::createList({}));

              if (mode == Types::ControlTypes::FLOW_CALL)
                setRecordValue(valueRecord, ConfigKeys::FLOW, maki::Value::createString(""));
              else if (mode == Types::ControlTypes::EVENT_SELECT)
                setRecordValue(valueRecord, ConfigKeys::EVENT, maki::Value::createString(""));

              const auto componentId = componentInfo.toStringList().at(0);
              setRecordValue(valueRecord, ConfigKeys::COMPONENT_ID, maki::Value::createString(componentId));
              setRecordValue(valueRecord, ConfigKeys::COMPONENT_TYPE, maki::Value::createString(componentInfo.toStringList().at(1)));
              setRecordValue(valueRecord, ConfigKeys::CALL_ID, maki::Value::createString(""));

              nodeInfo->setProperty(propertyId, maki::Value::createRecord(valueRecord));
              argumentsGroup->clear();

              if (callEditor)
              {
                populateCalls(componentId);
                callEditor->setValue(Constants::EMPTY_COMBO);
              }
              else
              {
                const auto call = findCall(componentId, QString());
                LOG_WARN_ON_FAILURE(loadCallArguments(call, propertyId, nodeId, argumentsGroup));
              }

              updateBlockName(nodeInfo, componentName, "");
            }
          });

  return VoidResult();
}

VoidResult PropertiesMenu::loadCallArguments(const std::shared_ptr<FlowSaveInfo>& call, const QString& propertyId, const QString& nodeId,
                                             maki::WidgetGroup* group)
{
  if (!group || nodeId.isEmpty())
    return VoidResult::Failed("Cannot load call arguments without a group and node");

  group->clear();

  if (!call || call->getarguments().isEmpty())
  {
    group->hide();
    return VoidResult();
  }

  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return VoidResult::Failed("Could not find node with id: {}", nodeId);

  group->show();

  const auto* property = nodeInfo->getProperty(propertyId);
  auto record = maki::parameterRecord(property);
  auto values = maki::recordList(record, ConfigKeys::ARGUMENTS);

  const auto arguments = call->getarguments();
  if (values.size() < static_cast<size_t>(arguments.size()))
    values.resize(arguments.size());

  for (int index = 0; index < arguments.size(); ++index)
  {
    const auto& argument = arguments[index];
    if (!argument)
      continue;

    maki::Value value = values[static_cast<size_t>(index)];
    if (!value.isValid())
    {
      const auto* defaultValue = maki::asValue(argument->getvalue());
      if (defaultValue && defaultValue->isValid())
        value = *defaultValue;
      else
        value = maki::Value::defaultValue(argument->gettype());

      values[static_cast<size_t>(index)] = value;
    }

    auto alignment = !argument->gettype().isPrimitive() ? maki::WidgetAlignment::Direction::ABOVE : maki::WidgetAlignment::Direction::SPREAD;
    auto* editor = maki::ValueEditorFactory::create(argument->getid(), argument->gettype(), value, maki::WidgetAlignment::Form(group, 70, alignment), this);
    editor->setObjectName(propertyId + QString("::arg:%1").arg(index));

    if (!editor)
    {
      LOG_WARNING("Could not create argument editor '{}' of type '{}'", argument->getid(), argument->gettype().toString());
      continue;
    }

    addCompleter(editor, nodeId, argument->gettype());

    connect(editor, &maki::InputWidget::valueChanged, this, [this, nodeInfo, propertyId, editor, index] {
      if (!editor)
        return;

      const auto* current = nodeInfo->getProperty(propertyId);
      auto valueRecord = maki::parameterRecord(current);
      auto arguments = maki::recordList(valueRecord, ConfigKeys::ARGUMENTS);

      if (arguments.size() <= static_cast<size_t>(index))
        arguments.resize(static_cast<size_t>(index) + 1);

      arguments[static_cast<size_t>(index)] = editor->getValue();
      maki::setRecordValue(valueRecord, ConfigKeys::ARGUMENTS, maki::Value::createList(arguments));
      auto propertyValue = maki::Value::createRecord(valueRecord);
      nodeInfo->setProperty(propertyId, propertyValue);

      emit propertyUpdated(propertyId, nodeInfo->getid(), propertyValue);
    });
    connect(editor, &maki::InputWidget::openParameter, this, [this](const QString& parameterId) { emit openParameter(parameterId); });
  }

  return VoidResult();
}

void PropertiesMenu::addCompleter(maki::InputWidget* editor, const QString& nodeId, const koda::types::TypeReference& type)
{
  if (!editor)
    return;

  if (auto* intEditor = qobject_cast<maki::IntegerWidget*>(editor))
    intEditor->setAcceptVariable(true);
  if (auto* realEditor = qobject_cast<maki::FloatWidget*>(editor))
    realEditor->setAcceptVariable(true);

  QVector<maki::MissionParameter> variables = {};  // std::move(additionalValues);
  for (const auto& parameter : SaveInfo::instance().missionParameters())
    if (parameter.gettype().structurallyEquals(type))
    {
      LOG_DEBUG("Adding variable: {} {}", parameter.getname(), maki::asValue(parameter.getvalue())->toReadable());
      variables.append(parameter);
    }

  editor->setSupportedReferences(variables);
}

VoidResult PropertiesMenu::loadControls(const QString& nodeId)
{
  if (nodeId.isEmpty())
    return VoidResult();

  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return VoidResult::Failed("Could not create controls for unknown node with id: {}", nodeId);

  const auto nodeConfig = ConfigurationTable::instance().get(nodeInfo->getnodeId());
  if (!nodeInfo)
    return VoidResult::Failed("Could not create controls for unknown node with id: {}", nodeId);

  auto eventsTable = loadEventTable(nodeId, nodeConfig);
  LOG_WARN_ON_FAILURE(eventsTable);

  for (const auto& control : nodeConfig->controls)
  {
    switch (control.type)
    {
      case Types::ControlTypes::ADD_EVENT:
        if (eventsTable)
          LOG_WARN_ON_FAILURE(loadControlAddEvent(eventsTable.Value(), nodeId));
        break;

      case Types::ControlTypes::LINK:
      case Types::ControlTypes::ADD_STATE:
      case Types::ControlTypes::ADD_FIELD:
      default:
        break;
    }
  }

  return VoidResult();
}

Result<QTableWidget*> PropertiesMenu::loadEventTable(const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig)
{
  if (nodeConfig->libraryType != Types::LibraryTypes::STRUCTURAL)
    return nullptr;

  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return Result<QTableWidget*>::Failed("Could not create event table for unknown node with id: {}", nodeId);

  const auto nodeFlows = nodeInfo->getflows();
  const auto nodeEvents = nodeInfo->getevents();

  if (nodeFlows.isEmpty() && nodeEvents.isEmpty())
    return nullptr;

  // Create it once
  auto* eventsTable = new QTableWidget(this);
  eventsTable->setObjectName("EventTable");
  eventsTable->setColumnCount(3);
  eventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  eventsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  eventsTable->setHorizontalHeaderLabels({tr("Name"), tr("Type"), tr("Arguments")});

  eventsTable->verticalHeader()->hide();
  eventsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  eventsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  eventsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

  for (const auto& event : nodeFlows + nodeEvents)
  {
    const auto concrete = std::dynamic_pointer_cast<FlowSaveInfo>(event);
    if (concrete)
      addEventToTable(eventsTable, eventsTable->rowCount(), concrete);
  }

  connect(eventsTable, &QTableWidget::doubleClicked, this,
          [this, eventsTable, nodeId](const QModelIndex& index) { (void)openEventDialog(eventsTable, nodeId, index.row()); });

  auto group = new maki::ContainerWidget(tr("Events/Flows"), eventsTable, maki::WidgetAlignment::Vertical(), this);
  // Set this after adding to the container so the container does not override it
  eventsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  eventsTable->setMaximumHeight(500);  // Let the table expand but not infinitelly
  layout()->addWidget(group);

  return eventsTable;
}

VoidResult PropertiesMenu::loadControlAddEvent(QTableWidget* table, const QString& nodeId)
{
  auto* browserButtons = new QHBoxLayout();
  auto* addButton = new QPushButton(this);
  addButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  addButton->setIcon(iconFromTheme("plus"));

  auto* removeButton = new QPushButton(this);
  removeButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  removeButton->setIcon(iconFromTheme("minus"));

  connect(addButton, &QPushButton::pressed, this, [this, nodeId] { emit flowCreationRequested(nodeId); });
  connect(removeButton, &QPushButton::clicked, this, [this, table, nodeId] { removeEventFromTable(table, nodeId); });

  browserButtons->addWidget(addButton);
  browserButtons->addWidget(removeButton);
  browserButtons->addStretch();

  if (auto* vLayout = qobject_cast<QVBoxLayout*>(layout()))
    vLayout->addLayout(browserButtons);

  return VoidResult();
}

std::shared_ptr<FlowSaveInfo> PropertiesMenu::getFlowConfigOfRow(PNodeSaveInfo nodeInfo, QTableWidget* table, int row) const
{
  if (!table || !nodeInfo || row >= table->rowCount())
    return nullptr;

  const auto item = table->item(row, 0);
  if (!item)
    return nullptr;

  const auto data = item->data(TableRole::IdRole);
  if (data.isNull())
    return nullptr;

  const auto flowId = data.toString();
  auto existing = maki::findBy(nodeInfo->getflows() + nodeInfo->getevents(), &IFlow::getid, flowId);
  if (existing)
    return std::dynamic_pointer_cast<FlowSaveInfo>(*existing);

  return nullptr;
}

void PropertiesMenu::openEventDialog(QTableWidget* table, const QString& nodeId, int row, bool synchronousCreation)
{
  if (!table)
    return;

  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return;

  auto* dialog = new EventDialog(tr("Edit event"), this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);

  auto flowConfig = getFlowConfigOfRow(nodeInfo, table, row);
  if (!flowConfig)
    return;

  dialog->setup(flowConfig, tr("Open"));
  connect(dialog, &QDialog::accepted, this, [this, dialog, nodeId] {
    auto info = dialog->getInfo();
    if (!info)
      return;

    emit flowSelected(info->getid(), nodeId);
  });

  dialog->open();
}

void PropertiesMenu::addEventToTable(QTableWidget* table, int row, const std::shared_ptr<FlowSaveInfo>& event)
{
  if (!table || !event)
    return;

  if (row >= table->rowCount())
    table->insertRow(row);

  auto* indexItem = new QTableWidgetItem(event->getname());
  indexItem->setData(TableRole::IdRole, event->getid());
  indexItem->setData(TableRole::ModifiableRole, event->getmodifiable());

  table->setItem(row, 0, indexItem);
  table->setItem(row, 1, new QTableWidgetItem(Types::CallTypeToString(event->gettype())));

  QStringList arguments;
  for (const auto& argument : event->getarguments())
    if (argument)
      arguments.append(argument->getid());

  table->setItem(row, 2, new QTableWidgetItem(arguments.join(", ")));
  table->adjustSize();
}

void PropertiesMenu::removeEventFromTable(QTableWidget* table, const QString& nodeId)
{
  if (nodeId.isEmpty())
    return;

  maki::applyOnSortedTable(table, [table, nodeId](const auto& row) {
    auto flowId = table->item(row, 0)->data(TableRole::IdRole).toString();
    table->removeRow(row);
    SaveInfo::instance().removeFlowFromNode(flowId, nodeId);
  });
}

VoidResult PropertiesMenu::onTransitionSelected(TransitionItem* transition)
{
  clearLayout(layout());

  if (!transition)
    return VoidResult();

  mCurrentNode = transition->id();

  auto source = transition->source();
  if (!source)
    return VoidResult::Failed("Transition with no source");

  auto* eventWidget = new maki::SelectorWidget(tr("Transition event"), maki::WidgetAlignment::Vertical(), this);

  for (const auto& configured : source->configTransitions())
    eventWidget->addItem(configured.event, configured.event);

  eventWidget->addItem("on error", "on error");
  eventWidget->addItem("on abort", "on abort");

  for (const auto& caller : SaveInfo::instance().getPossibleCallers(source->id(), Types::ControlTypes::AUTO))
  {
    if (!caller)
      continue;

    const auto* name = caller->getProperty(ConfigKeys::NAME);
    if (!name || !name->getvalue() || !name->getvalue()->isValid())
      continue;

    const auto componentName = name->getvalue()->toStringValue();
    for (const auto& event : SaveInfo::instance().getEventsOfTypeFromNode(caller->getid(), {Types::CallType::OUT}))
      if (event)
        eventWidget->addItem(componentName + "." + event->getname(), "on");
  }

  const auto currentEvent = transition->getEvent();
  BLOCK_AND_SET(eventWidget, currentEvent.isEmpty() ? Constants::EMPTY_COMBO : currentEvent);

  connect(eventWidget, &maki::SelectorWidget::dataChanged, this, [transition](const QString& text, const QVariant& data) {
    transition->setEvent(text);
    transition->setName(data.toString());
  });

  auto* resetButton = new QPushButton(tr("Reset"), this);
  connect(resetButton, &QPushButton::pressed, this, [transition, eventWidget] {
    transition->setEvent("");
    transition->setName("");
    eventWidget->setValue(Constants::EMPTY_COMBO);
  });

  layout()->addWidget(eventWidget);
  layout()->addWidget(resetButton);

  if (auto* vLayout = qobject_cast<QVBoxLayout*>(layout()))
    vLayout->addStretch();

  return VoidResult();
}

void PropertiesMenu::updateBlockName(PNodeSaveInfo nodeInfo, const QString& componentName, const QString& eventName) const
{
  if (!nodeInfo)
    return;

  LOG_DEBUG("AAAAAA");
  const auto* name = nodeInfo->getProperty(ConfigKeys::NAME);
  if (!name || !name->getvalue() || !name->getvalue()->isValid())
    return;

  const auto* autoGenerated = nodeInfo->getProperty("name_auto_generated");
  if (autoGenerated && autoGenerated->getvalue() && autoGenerated->getvalue()->isValid() && !autoGenerated->getvalue()->toBoolValue())
    return;

  const auto newName = componentName + (eventName.isEmpty() ? "" : " " + eventName);
  nodeInfo->setProperty(ConfigKeys::NAME, maki::Value::createString(newName));
}
