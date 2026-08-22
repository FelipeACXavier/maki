#include "properties_menu.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <algorithm>
#include <oclero/qlementine/widgets/Label.hpp>

#include "../dialogs/prompt.h"
#include "../structure/event_dialog.h"
#include "app_configs.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "keys.h"
#include "logging.h"
#include "mission_parameter.h"
#include "oclero/qlementine/Common.hpp"
#include "property_info.h"
#include "style_helpers.h"

namespace
{
constexpr auto VALUE_KEY = "value";
constexpr auto ITEMS_KEY = "items";
constexpr auto COMPONENT_KEY = "component";
constexpr auto EVENT_KEY = "event";
constexpr auto FLOW_KEY = "flow";
constexpr auto ARGUMENTS_KEY = "arguments";

const maki::Value* asValue(const IValue* value)
{
  return dynamic_cast<const maki::Value*>(value);
}

maki::Value parameterValue(const IParameter* parameter)
{
  if (!parameter)
    return {};

  const auto* value = asValue(parameter->getvalue());
  return value ? *value : maki::Value{};
}

maki::RecordValue parameterRecord(const IParameter* parameter)
{
  const auto value = parameterValue(parameter);
  return value.kind() == IValue::Kind::Record ? value.toRecord() : maki::RecordValue{};
}

QString recordString(const maki::RecordValue& record, const char* key)
{
  const auto it = record.find(key);
  if (it == record.end())
    return {};

  return it->second.toString();
}

maki::ListValue recordList(const maki::RecordValue& record, const char* key)
{
  const auto it = record.find(key);
  if (it == record.end() || it->second.kind() != IValue::Kind::List)
    return {};

  return it->second.toList();
}

bool hasRecordField(const maki::RecordValue& record, const char* key)
{
  return record.find(key) != record.end();
}

Types::ControlTypes callControl(const maki::RecordValue& record)
{
  if (hasRecordField(record, FLOW_KEY))
    return Types::ControlTypes::FLOW_CALL;

  if (hasRecordField(record, EVENT_KEY))
    return Types::ControlTypes::EVENT_SELECT;

  return Types::ControlTypes::TRIGGER_CALL;
}

}  // namespace

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

  mFrame = new StyledFrame(this);
  mFrame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  mFrame->setBorderRole(StyledFrame::BorderRole::Mid);
  mFrame->setRadius(theme.borderRadius);
  mFrame->setBorderWidth(theme.borderWidth);
  mFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  auto* frameLayout = new QVBoxLayout(mFrame);
  frameLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  frameLayout->setSpacing(theme.spacing);

  rootLayout->addWidget(mFrame);
}

QLayout* PropertiesMenu::layout() const
{
  return mFrame ? mFrame->layout() : QFrame::layout();
}

VoidResult PropertiesMenu::start(std::shared_ptr<SaveInfo> storage)
{
  mStorage = std::move(storage);
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeAdded(NodeItem* /* node */)
{
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

VoidResult PropertiesMenu::onNodeModified(NodeItem* /* node */)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeSelected(NodeItem* node, bool /* selected */)
{
  clearLayout(layout());

  if (!node)
    return VoidResult();

  mCurrentNode = node->id();

  RETURN_ON_FAILURE(loadProperties(node));
  RETURN_ON_FAILURE(loadControls(node));

  if (auto* vLayout = qobject_cast<QVBoxLayout*>(layout()))
    vLayout->addStretch();

  return VoidResult();
}

VoidResult PropertiesMenu::onCreateEvent(NodeItem* node)
{
  onNodeSelected(node, true);

  auto table = findChild<QTableWidget*>("EventTable");
  if (table == nullptr)
    return VoidResult::Failed("Could not find the event table");

  openEventDialog(table, node, table->rowCount());

  return VoidResult();
}

VoidResult PropertiesMenu::onFlowAdded(Flow* /* flow */, NodeItem* node)
{
  if (node->id() != mCurrentNode)
    return VoidResult();

  return onNodeSelected(node, true);
}

VoidResult PropertiesMenu::onFlowRemoved(const QString& /* flowId */, NodeItem* node)
{
  if (node->id() != mCurrentNode)
    return VoidResult();

  return onNodeSelected(node, true);
}

VoidResult PropertiesMenu::loadProperties(NodeItem* node)
{
  if (!node)
    return VoidResult::Failed("Cannot load properties for a null node");

  for (const auto& property : node->properties())
  {
    if (!property)
      continue;

    LOG_TRACE("Loading property: {} {} {}", property->getname(), dynamic_cast<const maki::Value*>(property->getvalue())->toReadable(),
              (int)property->getcontrol());
    switch (property->getcontrol())
    {
      case Types::ControlTypes::SELECT:
      case Types::ControlTypes::EVENT_SELECT:
        LOG_WARN_ON_FAILURE(loadSelectProperty(property, node));
        break;

      case Types::ControlTypes::COLOR:
        LOG_WARN_ON_FAILURE(loadColorProperty(property, node));
        break;

      case Types::ControlTypes::COMPONENT_SELECT:
        LOG_WARN_ON_FAILURE(loadComponentSelectProperty(property, node));
        break;

      default:
        LOG_WARN_ON_FAILURE(loadValueProperty(property, node));
        break;
    }
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadValueProperty(const std::shared_ptr<IParameter>& property, NodeItem* node)
{
  if (!property || !node)
    return VoidResult::Failed("Cannot create property editor without a property and node");

  const auto* storedValue = asValue(property->getvalue());
  if (!storedValue)
    return VoidResult::Failed("Property '{}' does not contain a MAKI Value", property->getid().toStdString());

  // Void is used for special types and should not be added here
  if (property->gettype().isPrimitive() && property->gettype().primitiveKind() == koda::types::PrimitiveKind::Void)
    return VoidResult();

  auto* editor =
      maki::ValueEditorFactory::create(ToLabel(property->getid()), property->gettype(), *storedValue, maki::WidgetAlignment::Vertical(), this);
  if (!editor)
    return VoidResult::Failed("Could not create editor for property '{}' of type '{}'", property->getid().toStdString(),
                              property->gettype().toString());

  addCompleter(editor, node->id(), property->gettype());

  connect(editor, &maki::InputWidget::valueChanged, this, [node, editor, id = property->getid()] {
    if (!node || !editor)
      return;

    auto value = editor->getValue();

    // Preserve the old automatic-name behaviour, but use Value throughout.
    if (id == ConfigKeys::NAME)
    {
      const auto text = value.toString();
      const bool automaticallyGenerated = text.isEmpty();

      if (automaticallyGenerated)
        node->setProperty(id, maki::Value::createString(node->nodeType().replace("Koda::", "")));
      else
        node->setProperty(id, value);

      if (node->getProperty("name_auto_generated"))
        node->setProperty("name_auto_generated", maki::Value::createBool(automaticallyGenerated));

      return;
    }

    node->setProperty(id, value);
  });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadColorProperty(const std::shared_ptr<IParameter>& property, NodeItem* node)
{
  if (!property || !node)
    return VoidResult::Failed("Cannot create color editor without a property and node");

  const auto value = parameterValue(property.get());
  auto* editor = new maki::ColorWidget(ToLabel(property->getid()), "", maki::WidgetAlignment::Vertical(), this);

  if (value.kind() == IValue::Kind::Color)
    editor->setValue(std::get<QColor>(value.data));
  else if (value.isValid())
    editor->setValue(QColor::fromString(value.toString()));

  connect(editor, &maki::ColorWidget::valueChanged, this, [node, id = property->getid()](const QColor& color) {
    if (!node)
      return;

    maki::Value value;
    value.data = color;
    node->setProperty(id, value);
  });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadSelectProperty(const std::shared_ptr<IParameter>& property, NodeItem* node)
{
  if (!property || !node)
    return VoidResult::Failed("Cannot create selector without a property and node");

  auto record = parameterRecord(property.get());
  if (!hasRecordField(record, VALUE_KEY) || !hasRecordField(record, ITEMS_KEY))
    return VoidResult::Failed("Select property '{}' must be a record containing 'value' and 'items'", property->getid().toStdString());

  const auto items = recordList(record, ITEMS_KEY);
  const auto selected = record.at(VALUE_KEY);

  auto* editor = new maki::SelectorWidget(ToLabel(property->getid()), maki::WidgetAlignment::Vertical(), this);
  for (std::size_t i = 0; i < items.size(); ++i)
    editor->addItem(items[i].toString(), static_cast<qulonglong>(i));

  editor->setValue(selected.toString());

  connect(editor, &maki::SelectorWidget::dataChanged, this,
          [node, id = property->getid(), record, items](const QString& /* text */, const QVariant& data) mutable {
            if (!node || !data.isValid())
              return;

            const auto index = data.toULongLong();
            if (index >= items.size())
              return;

            record[VALUE_KEY] = items[index];
            node->setProperty(id, maki::Value::createRecord(record));
          });

  layout()->addWidget(editor);
  return VoidResult();
}

VoidResult PropertiesMenu::loadComponentSelectProperty(const std::shared_ptr<IParameter>& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");
  if (!property || !node)
    return VoidResult::Failed("Cannot create component selector without a property and node");

  auto record = parameterRecord(property.get());
  if (!hasRecordField(record, COMPONENT_KEY))
    return VoidResult::Failed("Component-select property '{}' must contain a 'component' field", property->getid().toStdString());

  const auto mode = callControl(record);
  LOG_DEBUG("Using control mode: {}", Types::ControlTypesToString(mode));
  auto* componentEditor = new maki::SelectorWidget(ToLabel(property->getid()), maki::WidgetAlignment::Vertical(), this);

  for (const auto& candidate : mStorage->getPossibleCallers(node->id(), mode))
  {
    if (!candidate)
      continue;

    const auto* name = candidate->getProperty(ConfigKeys::NAME);
    if (!name || !name->getvalue() || !name->getvalue()->isValid())
      continue;

    componentEditor->addItem(name->getvalue()->toStringValue(), candidate->getid());
  }

  componentEditor->setValue(recordString(record, COMPONENT_KEY));
  layout()->addWidget(componentEditor);

  auto* callEditor = static_cast<maki::SelectorWidget*>(nullptr);
  auto* argumentsGroup = new maki::WidgetGroup(tr("Arguments"), oclero::qlementine::TextRole::Default, this);

  const bool hasNamedCall = mode == Types::ControlTypes::EVENT_SELECT || mode == Types::ControlTypes::FLOW_CALL;
  if (hasNamedCall)
  {
    const QString label = mode == Types::ControlTypes::FLOW_CALL ? tr("Flow") : tr("Event");
    callEditor = new maki::SelectorWidget(label, maki::WidgetAlignment::Vertical(), this);
    layout()->addWidget(callEditor);
  }

  layout()->addWidget(argumentsGroup);

  auto findCall = [this, mode](const QString& componentId, const QString& callName) -> std::shared_ptr<FlowSaveInfo> {
    if (componentId.isEmpty())
      return nullptr;

    if (mode == Types::ControlTypes::FLOW_CALL)
      return mStorage->getFlowFromNode(componentId, callName);

    if (mode == Types::ControlTypes::EVENT_SELECT)
      return mStorage->getEventFromNode(componentId, callName);

    const auto events = mStorage->getEventsOfTypeFromNode(componentId, {Types::CallType::TRIGGER});
    return events.isEmpty() ? nullptr : events.first();
  };

  auto populateCalls = [this, mode, callEditor](const QString& componentId) {
    if (!callEditor)
      return;

    callEditor->widget()->clear();

    if (mode == Types::ControlTypes::FLOW_CALL)
    {
      for (const auto& flow : mStorage->getFlowsFromNode(componentId))
        if (flow)
          callEditor->addItem(flow->getname(), flow->getname());
    }
    else
    {
      for (const auto& event : mStorage->getEventsOfTypeFromNode(componentId, {Types::CallType::TRIGGER, Types::CallType::ABORT}))
        if (event)
          callEditor->addItem(event->getname(), event->getname());
    }
  };

  // Initialise dependent fields before connecting change handlers.
  const auto currentComponentId = componentEditor->getData().toString();
  if (callEditor)
  {
    populateCalls(currentComponentId);
    callEditor->setValue(recordString(record, mode == Types::ControlTypes::FLOW_CALL ? FLOW_KEY : EVENT_KEY));
  }

  auto currentCall = findCall(currentComponentId, mode == Types::ControlTypes::FLOW_CALL      ? recordString(record, FLOW_KEY)
                                                  : mode == Types::ControlTypes::EVENT_SELECT ? recordString(record, EVENT_KEY)
                                                                                              : QString());
  LOG_WARN_ON_FAILURE(loadCallArguments(currentCall, property->getid(), node, argumentsGroup));

  if (callEditor)
  {
    connect(callEditor, &maki::SelectorWidget::valueChanged, this,
            [this, node, propertyId = property->getid(), componentEditor, argumentsGroup, findCall, mode](const QString& callName) {
              if (!node || callName.isEmpty())
                return;

              const auto* current = node->getProperty(propertyId);
              auto valueRecord = parameterRecord(current);
              const char* key = mode == Types::ControlTypes::FLOW_CALL ? FLOW_KEY : EVENT_KEY;
              valueRecord[key] = maki::Value::createString(callName);
              valueRecord[ARGUMENTS_KEY] = maki::Value::createList({});
              node->setProperty(propertyId, maki::Value::createRecord(valueRecord));

              argumentsGroup->clear();
              const auto call = findCall(componentEditor->getData().toString(), callName);
              LOG_WARN_ON_FAILURE(loadCallArguments(call, propertyId, node, argumentsGroup));

              updateBlockName(node, componentEditor->getValue(), callName);
            });
  }

  connect(componentEditor, &maki::SelectorWidget::dataChanged, this,
          [this, node, propertyId = property->getid(), callEditor, argumentsGroup, populateCalls, findCall, mode](const QString& componentName,
                                                                                                                  const QVariant& componentId) {
            if (!node || !componentId.isValid())
              return;

            const auto* current = node->getProperty(propertyId);
            auto valueRecord = parameterRecord(current);
            valueRecord[COMPONENT_KEY] = maki::Value::createString(componentName);
            valueRecord[ARGUMENTS_KEY] = maki::Value::createList({});

            if (mode == Types::ControlTypes::FLOW_CALL)
              valueRecord[FLOW_KEY] = maki::Value::createString("");
            else if (mode == Types::ControlTypes::EVENT_SELECT)
              valueRecord[EVENT_KEY] = maki::Value::createString("");

            node->setProperty(propertyId, maki::Value::createRecord(valueRecord));

            argumentsGroup->clear();

            if (callEditor)
            {
              populateCalls(componentId.toString());
              callEditor->setValue(Constants::EMPTY_COMBO);
            }
            else
            {
              const auto call = findCall(componentId.toString(), QString());
              LOG_WARN_ON_FAILURE(loadCallArguments(call, propertyId, node, argumentsGroup));
            }

            updateBlockName(node, componentName, "");
          });

  return VoidResult();
}

VoidResult PropertiesMenu::loadCallArguments(const std::shared_ptr<FlowSaveInfo>& call, const QString& propertyId, NodeItem* node,
                                             maki::WidgetGroup* group)
{
  if (!group || !node)
    return VoidResult::Failed("Cannot load call arguments without a group and node");

  group->clear();

  if (!call || call->getarguments().isEmpty())
  {
    group->hide();
    return VoidResult();
  }

  group->show();

  const auto* property = node->getProperty(propertyId);
  auto record = parameterRecord(property);
  auto values = recordList(record, ARGUMENTS_KEY);

  const auto arguments = call->getarguments();
  if (values.size() < static_cast<std::size_t>(arguments.size()))
    values.resize(arguments.size());

  for (int index = 0; index < arguments.size(); ++index)
  {
    const auto& argument = arguments[index];
    if (!argument)
      continue;

    maki::Value value = values[static_cast<std::size_t>(index)];
    if (!value.isValid())
    {
      const auto* defaultValue = asValue(argument->getvalue());
      if (defaultValue)
        value = *defaultValue;
      else
        value = maki::Value::defaultValue(argument->gettype());

      values[static_cast<std::size_t>(index)] = value;
    }

    auto* editor = maki::ValueEditorFactory::create(argument->getid(), argument->gettype(), value, maki::WidgetAlignment::Form(group, 70), this);
    if (!editor)
    {
      LOG_WARNING("Could not create argument editor '{}' of type '{}'", argument->getid(), argument->gettype().toString());
      continue;
    }

    addCompleter(editor, node->id(), argument->gettype());

    connect(editor, &maki::InputWidget::valueChanged, this, [node, propertyId, editor, index] {
      if (!node || !editor)
        return;

      const auto* current = node->getProperty(propertyId);
      auto valueRecord = parameterRecord(current);
      auto arguments = recordList(valueRecord, ARGUMENTS_KEY);

      if (arguments.size() <= static_cast<std::size_t>(index))
        arguments.resize(static_cast<std::size_t>(index) + 1);

      arguments[static_cast<std::size_t>(index)] = editor->getValue();
      valueRecord[ARGUMENTS_KEY] = maki::Value::createList(arguments);
      node->setProperty(propertyId, maki::Value::createRecord(valueRecord));
    });
  }

  // Store defaulted values immediately, so the record always matches the call signature.
  record[ARGUMENTS_KEY] = maki::Value::createList(values);
  node->setProperty(propertyId, maki::Value::createRecord(record));

  return VoidResult();
}

void PropertiesMenu::addCompleter(maki::InputWidget* editor, const QString& nodeId, const koda::types::TypeReference& type,
                                  QStringList additionalValues)
{
  if (!editor || !mStorage)
    return;

  auto variables = std::move(additionalValues);

  for (const auto& state : mStorage->getPossibleStates(nodeId))
  {
    if (!state)
      continue;

    if (state->gettype().structurallyEquals(type))
      variables.append(state->getid());
  }

  variables.removeDuplicates();
  if (variables.isEmpty())
    return;

  for (auto* widget : editor->focusWidgets())
    if (widget)
      maki::addCompleter(variables, widget);
}

VoidResult PropertiesMenu::loadControls(NodeItem* node)
{
  if (!node)
    return VoidResult();

  auto eventsTable = loadEventTable(node);
  LOG_WARN_ON_FAILURE(eventsTable);

  for (const auto& control : node->controls())
  {
    switch (control.type)
    {
      case Types::ControlTypes::ADD_EVENT:
        if (eventsTable)
          LOG_WARN_ON_FAILURE(loadControlAddEvent(eventsTable.Value(), node));
        break;

      case Types::ControlTypes::ADD_STATE:
      case Types::ControlTypes::ADD_FIELD:
      default:
        break;
    }
  }

  return VoidResult();
}

Result<QTableWidget*> PropertiesMenu::loadEventTable(NodeItem* node)
{
  if (node->function() != Types::LibraryTypes::STRUCTURAL)
    return nullptr;

  if (node->flowConfigs().isEmpty() && node->eventConfigs().isEmpty())
    return nullptr;

  // Create it once
  auto* eventsTable = new QTableWidget(this);
  eventsTable->setObjectName("EventTable");
  eventsTable->setColumnCount(4);
  eventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  eventsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  eventsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  eventsTable->setHorizontalHeaderLabels({"Name", "Type", "Return", "Arguments"});
  eventsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  eventsTable->verticalHeader()->hide();
  eventsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  eventsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  eventsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  eventsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

  for (const auto& event : node->allEventFlowConfigs())
  {
    const auto concrete = std::dynamic_pointer_cast<FlowSaveInfo>(event);
    if (concrete)
      addEventToTable(eventsTable, eventsTable->rowCount(), concrete);
  }

  connect(eventsTable, &QTableWidget::doubleClicked, this,
          [this, eventsTable, node](const QModelIndex& index) { openEventDialog(eventsTable, node, index.row()); });

  auto group = new maki::ContainerWidget(tr("Events/Flows"), eventsTable, maki::WidgetAlignment::Vertical(), this);
  layout()->addWidget(group);

  return eventsTable;
}

VoidResult PropertiesMenu::loadControlAddEvent(QTableWidget* table, NodeItem* node)
{
  auto* browserButtons = new QHBoxLayout();
  auto* addButton = new QPushButton(this);
  addButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  addButton->setIcon(iconFromTheme("plus"));

  auto* removeButton = new QPushButton(this);
  removeButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  removeButton->setIcon(iconFromTheme("minus"));

  connect(addButton, &QPushButton::pressed, this, [this, table, node] {
    if (table)
      openEventDialog(table, node, table->rowCount());
  });
  connect(removeButton, &QPushButton::clicked, this, [this, table, node] { removeEventFromTable(table, node); });

  browserButtons->addWidget(addButton);
  browserButtons->addWidget(removeButton);
  browserButtons->addStretch();

  if (auto* vLayout = qobject_cast<QVBoxLayout*>(layout()))
    vLayout->addLayout(browserButtons);

  return VoidResult();
}

std::shared_ptr<IFlow> PropertiesMenu::getFlowConfigOfRow(const NodeItem* node, QTableWidget* table, int row) const
{
  if (!table || !node || row >= table->rowCount())
    return nullptr;

  const auto item = table->item(row, 0);
  if (!item)
    return nullptr;

  const auto data = item->data(TableRole::IdRole);
  if (data.isNull())
    return nullptr;

  const auto flowId = data.toString();
  for (const auto& flowConfig : node->allEventFlowConfigs())
    if (flowConfig->getid() == flowId)
      return flowConfig;

  return nullptr;
}

void PropertiesMenu::openEventDialog(QTableWidget* table, NodeItem* node, int row)
{
  if (!table || !node)
    return;

  EventDialog dialog(tr("Edit event"), this);

  auto flowConfig = getFlowConfigOfRow(node, table, row);
  auto config = flowConfig ? std::dynamic_pointer_cast<FlowSaveInfo>(flowConfig) : std::make_shared<FlowSaveInfo>();
  dialog.setup(config);
  if (dialog.exec() != QDialog::Accepted)
    return;

  auto info = dialog.getInfo();
  if (!info)
    return;

  for (const auto& f : node->allEventFlowConfigs())
    if (f->getname() == info->getname())
    {
      if (maki::errorPrompt(tr("A flow with the same name already exists"), tr("Would you like to open it?")))
        emit flowSelected(f->getid(), node->id());
      return;
    }

  Flow* flow = node->createFlow(info->getname(), info);
  addEventToTable(table, row, info);

  if (flow && info->getmodifiable())
    emit flowSelected(flow->id(), node->id());
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
  table->setItem(row, 2, new QTableWidgetItem(Types::PropertyTypesToString(event->getreturnType())));

  QStringList arguments;
  for (const auto& argument : event->getarguments())
    if (argument)
      arguments.append(argument->getid());

  table->setItem(row, 3, new QTableWidgetItem(arguments.join(", ")));
}

void PropertiesMenu::removeEventFromTable(QTableWidget* table, NodeItem* node)
{
  if (!table || !node)
  {
    LOG_DEBUG("Cannot remove events with no table or node");
    return;
  }

  const auto selectedRows = table->selectionModel()->selectedRows();
  if (selectedRows.empty())
  {
    LOG_DEBUG("No rows selected for removal");
    return;
  }

  std::vector<int> rows;
  rows.reserve(selectedRows.size());

  for (const auto& index : selectedRows)
    rows.push_back(index.row());

  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const int row : rows)
  {
    if (row < 0 || row >= table->rowCount())
      continue;

    auto flowId = table->item(row, 0)->data(TableRole::IdRole).toString();
    table->removeRow(row);
    emit flowRemoved(flowId, node->id());
  }
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

  if (mStorage)
  {
    for (const auto& caller : mStorage->getPossibleCallers(source->id(), Types::ControlTypes::AUTO))
    {
      if (!caller)
        continue;

      const auto* name = caller->getProperty(ConfigKeys::NAME);
      if (!name || !name->getvalue() || !name->getvalue()->isValid())
        continue;

      const auto componentName = name->getvalue()->toStringValue();
      for (const auto& event : mStorage->getEventsOfTypeFromNode(caller->getid(), {Types::CallType::OUT}))
        if (event)
          eventWidget->addItem(componentName + "." + event->getname(), "on");
    }
  }

  const auto currentEvent = transition->getEvent();
  eventWidget->setValue(currentEvent.isEmpty() ? Constants::EMPTY_COMBO : currentEvent);

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

void PropertiesMenu::updateBlockName(NodeItem* node, const QString& componentName, const QString& eventName) const
{
  if (!node)
    return;

  const auto* name = node->getProperty(ConfigKeys::NAME);
  if (!name || !name->getvalue() || !name->getvalue()->isValid())
    return;

  const auto* autoGenerated = node->getProperty("name_auto_generated");
  if (autoGenerated && autoGenerated->getvalue() && autoGenerated->getvalue()->isValid() && !autoGenerated->getvalue()->toBoolValue())
    return;

  const auto newName = componentName + (eventName.isEmpty() ? "" : " " + eventName);
  node->setProperty(ConfigKeys::NAME, maki::Value::createString(newName));
}