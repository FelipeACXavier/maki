#include "system_menu.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>

#include "app_configs.h"
#include "config_table.h"
#include "document/save_info.h"
#include "keys.h"
#include "logging.h"
#include "style_helpers.h"

static const int ICON_COLUMN = 0;
static const int NAME_COLUMN = 0;
static const int TYPE_COLUMN = 1;

static const int ID_DATA = 0;
static const int TYPE_DATA = 1;
static const int CANVAS_DATA = 2;

static const QString PIPELINE_ROOT = "pipeline_root_id";
static constexpr int MIN_COLUMN_WIDTH = 75;

SystemMenu::SystemMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  setColumnCount(2);
  setIndentation(15);
  setExpandsOnDoubleClick(false);
  setHeaderLabels({tr("Name"), tr("Type")});

  header()->setMinimumSectionSize(MIN_COLUMN_WIDTH);
  header()->setStretchLastSection(false);
  header()->setAlternatingRowColors(true);

  header()->setSectionResizeMode(NAME_COLUMN, QHeaderView::Interactive);
  header()->setSectionResizeMode(TYPE_COLUMN, QHeaderView::Interactive);

  setColumnWidth(NAME_COLUMN, 220);

  header()->setTextElideMode(Qt::ElideRight);
  header()->setSectionsMovable(false);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  connect(this, &QTreeWidget::customContextMenuRequested, this, &SystemMenu::showContextMenu);
  connect(this, &QTreeWidget::itemDoubleClicked, this, &SystemMenu::onItemClicked);
  connect(header(), &QHeaderView::sectionResized, this, [this](int logicalIndex, int /*oldSize*/, int newSize) {
    if (logicalIndex != NAME_COLUMN && logicalIndex != TYPE_COLUMN)
      return;

    resizeColumns(logicalIndex, newSize);
  });
}

VoidResult SystemMenu::start()
{
  mDocumentSubscription = SaveInfo::instance().subscribe([this](const DocumentEvent& event) {
    LOG_DEBUG("Document event: {}", maki::toString(event.type));
    switch (event.type)
    {
      case DocumentEvent::Type::NodeAdded:
      {
        const auto* data = std::get_if<NodeEvent>(&event.data);
        if (!data)
          return;

        if (const auto config = ConfigurationTable::instance().get(data->nodeType))
          LOG_WARN_ON_FAILURE(onNodeAdded(data->ownerFlowId, data->id, config->libraryType));

        break;
      }
      case DocumentEvent::Type::NodeRemoved:
      {
        const auto* data = std::get_if<NodeEvent>(&event.data);
        if (!data)
          return;

        onNodeRemoved(data->ownerFlowId, data->id, data->parentId);
        break;
      }
      case DocumentEvent::Type::NodeModified:
      {
        const auto* data = std::get_if<NodeEvent>(&event.data);
        if (!data)
          return;

        onNodeModified(data->ownerFlowId, data->id);
        break;
      }
      case DocumentEvent::Type::FlowAdded:
      {
        const auto* data = std::get_if<FlowEvent>(&event.data);
        if (!data)
          return;

        LOG_WARN_ON_FAILURE(onFlowAdded(data->id, data->ownerNodeId));
        break;
      }
      case DocumentEvent::Type::FlowRemoved:
      {
        const auto* data = std::get_if<FlowEvent>(&event.data);
        if (!data)
          return;

        LOG_WARN_ON_FAILURE(onFlowRemoved(data->id, data->ownerNodeId));
        break;
      }
      default:
        break;
    }
  });
  return VoidResult();
}

VoidResult SystemMenu::onNodeAdded(const QString& flowId, const QString& nodeId, const Types::LibraryTypes canvasType)
{
  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return VoidResult::Failed("Could not find node with id: {}", nodeId);

  LOG_DEBUG("Node added to system menu: {} ({}) - Flow: {}", nodeInfo->getid(), nodeInfo->getnodeId(), flowId);
  const auto* exists = getItemById(nodeInfo->getid());
  if (exists)
    return VoidResult();

  // Check if we are adding a structural node
  if (flowId == Config::MAIN_CANVAS)
  {
    const auto parentId = nodeInfo->getparentId();
    if (parentId.isEmpty())
    {
      LOG_DEBUG("{} is a root node", nodeInfo->getnodeId());
      auto result = addRootNode(nodeInfo);
      return result;
    }

    auto result = addLeafNode(nodeInfo);
    return result;
  }

  auto parent = getItemById(flowId);
  if (parent == nullptr)
    return VoidResult::Failed("Could not add node: {}. Could not find parent flow", nodeId);

  QTreeWidgetItem* newNode = new QTreeWidgetItem(parent);
  populateItem(newNode, QIcon(":/icons/flow_block.svg"), nodeInfo->getNodeName(), nodeInfo->getnodeId(), nodeInfo->getid(),
               canvasType == Types::LibraryTypes::BEHAVIOUR ? Roles::NodeRole : Roles::PipelineRole, flowId);

  return VoidResult();
}

VoidResult SystemMenu::onNodeRemoved(const QString& flowId, const QString& nodeId, const QString& parentId)
{
  LOG_DEBUG("Node removed: {} ({})", nodeId, flowId);
  if (flowId == Config::MAIN_CANVAS)
  {
    auto* item = getItemById(nodeId);
    if (!item)
      return VoidResult::Failed("Node is not in the tree");

    if (parentId.isEmpty())
    {
      takeTopLevelItem(indexOfTopLevelItem(item));
      delete item;
      return VoidResult();
    }

    auto* group = item->parent();
    if (!group)
      return VoidResult::Failed("Node has no parent group");

    group->removeChild(item);
    delete item;

    removeGroupIfEmpty(group);
  }
  else
  {
    auto* flow = getItemById(flowId);
    if (!flow)
      return VoidResult::Failed("Flow is not in the tree");

    auto* component = getItemById(nodeId);
    if (!component)
      return VoidResult::Failed("The node is not in the tree");

    if (component->data(TYPE_DATA, Qt::UserRole) != Roles::NodeRole)
      return VoidResult::Failed("Item is in the tree but it is not a node");

    flow->removeChild(component);
    delete component;
  }

  return VoidResult();
}

VoidResult SystemMenu::onNodeModified(const QString& flowId, const QString& nodeId)
{
  auto item = getItemById(nodeId);
  if (!item)
    return VoidResult::Failed("Modified item is not in the tree");

  const auto nodeInfo = SaveInfo::instance().getNodeWithId(nodeId);
  if (!nodeInfo)
    return VoidResult::Failed("Could not find node with id: {}", nodeId);

  populateItem(item, item->icon(ICON_COLUMN), nodeInfo->getNodeName(), nodeInfo->getnodeId(), nodeInfo->getid(), Roles::NodeRole);

  return VoidResult();
}

VoidResult SystemMenu::onFlowAdded(const QString& flowId, const QString& nodeId)
{
  const auto flow = SaveInfo::instance().getFlowWithId(flowId);
  if (!flow)
    return VoidResult();

  if (!flow->getmodifiable())
    return VoidResult();

  const auto* exists = getItemById(flow->getid());
  if (exists)
    return VoidResult();

  LOG_INFO("Adding flow: {} ({}) to {}", flow->getname(), flow->getid(), nodeId);

  auto parent = getOrCreateChildGroup(nodeId, Roles::Flows);
  if (!parent)
    return VoidResult::Failed("Tried to add flow with no parent node");

  QTreeWidgetItem* newFlow = new QTreeWidgetItem(parent);

  // Assign the tree information
  populateItem(newFlow, QIcon(":/icons/flow.svg"), flow->getname(), tr("Flow"), flow->getid(), Roles::FlowRole, flow->getid());

  for (const auto& component : flow->getnodes())
  {
    if (const auto& node = std::dynamic_pointer_cast<NodeSaveInfo>(component); node)
    {
      auto nodeName = node->getNodeName();
      QTreeWidgetItem* newComponent = new QTreeWidgetItem(newFlow);
      populateItem(newComponent, QIcon(":/icons/flow_block.svg"), nodeName, component->getnodeId(), component->getid(), Roles::NodeRole, flow->getid());
    }
  }

  return VoidResult();
}

void collectTreeItems(QTreeWidgetItem* item, QSet<QTreeWidgetItem*>& out)
{
  if (!item)
    return;

  out.insert(item);
  for (int i = 0; i < item->childCount(); ++i)
    collectTreeItems(item->child(i), out);
}

VoidResult SystemMenu::onFlowRemoved(const QString& flowId, const QString& nodeId)
{
  const auto* nodeItem = getItemById(nodeId);
  if (!nodeItem)
    return VoidResult();

  auto* flowItem = getItemById(flowId);
  if (!flowItem)
    return VoidResult();

  auto* group = flowItem->parent();
  if (group)
    group->removeChild(flowItem);

  delete flowItem;
  removeGroupIfEmpty(group);

  return VoidResult();
}

VoidResult SystemMenu::onPipelineAdded(std::shared_ptr<const FlowSaveInfo> info)
{
  if (!info)
    return VoidResult::Failed("No pipeline info provided");

  auto existing = getItemById(info->getid());
  if (existing)
  {
    existing->setText(NAME_COLUMN, info->getname());
    return VoidResult();
  }

  LOG_DEBUG("Pipeline added to system menu: {} - {}", info->getid(), info->getname());

  auto* pipelineRoot = getOrCreateGroup(PIPELINE_ROOT, Roles::PipelineRoot, nullptr);
  if (!pipelineRoot)
    return VoidResult::Failed("Could not create pipeline root");

  auto* item = new QTreeWidgetItem(pipelineRoot);
  populateItem(item, QIcon(":/icons/pipeline_item.svg"), info->getname(), "Pipeline", info->getid(), Roles::Pipeline);

  return VoidResult();
}

VoidResult SystemMenu::onPipelineRemoved(const QString& pipelineId)
{
  LOG_DEBUG("Pipeline removed from system menu: {}", pipelineId);

  auto pipelineRoot = getItemById(PIPELINE_ROOT);
  if (pipelineRoot == nullptr)
    return VoidResult();

  auto pipelineItem = getItemById(pipelineId);
  if (pipelineItem == nullptr)
    return VoidResult();

  pipelineRoot->removeChild(pipelineItem);

  removeGroupIfEmpty(pipelineRoot);

  return VoidResult();
}

void SystemMenu::populateItem(QTreeWidgetItem* item, const QIcon& icon, const QString& name, const QString& type, const QString& data, const Roles role,
                              const QString& canvas)
{
  item->setIcon(ICON_COLUMN, icon);
  item->setText(NAME_COLUMN, name);
  item->setText(TYPE_COLUMN, type);
  item->setData(ID_DATA, Qt::UserRole, data);  // Not shown to user
  item->setData(TYPE_DATA, Qt::UserRole, role);
  if (!canvas.isEmpty())
    item->setData(CANVAS_DATA, Qt::UserRole, canvas);
}

void SystemMenu::populateTaskItem(QTreeWidgetItem* item, const QString& nodeId)
{
  QTreeWidgetItem* tasks = new QTreeWidgetItem(item);
  populateItem(tasks, QIcon(":/icons/structure.svg"), tr("Sub-tasks"), "", nodeId, Roles::SubTasks);

  QTreeWidgetItem* capabilities = new QTreeWidgetItem(item);
  populateItem(capabilities, QIcon(":/icons/structure.svg"), tr("Capabilities"), "", nodeId, Roles::Capabilities);

  QTreeWidgetItem* flows = new QTreeWidgetItem(item);
  populateItem(flows, QIcon(":/icons/behaviour.svg"), tr("Flows"), "", nodeId, Roles::Flows);
}

VoidResult SystemMenu::addRootNode(PNodeSaveInfo nodeInfo)
{
  // Root nodes are composed of two subnodes, Capabilities and Flows
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  populateItem(item, QIcon(":/icons/task.svg"), nodeInfo->getNodeName(), nodeInfo->getnodeId(), nodeInfo->getid(), Roles::NodeRole);
  addTopLevelItem(item);

  // populateTaskItem(item, node);

  return VoidResult();
}

VoidResult SystemMenu::addLeafNode(PNodeSaveInfo nodeInfo)
{
  const auto parentId = nodeInfo->getparentId();
  if (parentId.isEmpty())
    return VoidResult::Failed("No parent found, this should have been a root");

  auto parentItem = getOrCreateChildGroup(parentId, nodeInfo->getnodeId() == ConfigKeys::TASK_NODE ? Roles::SubTasks : Roles::Capabilities);
  if (!parentItem)
    return VoidResult::Failed("The parent node is not on the tree");

  QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
  populateItem(item, QIcon(":/icons/capability.svg"), nodeInfo->getNodeName(), nodeInfo->getnodeId(), nodeInfo->getid(), Roles::NodeRole);

  return VoidResult();
}

QTreeWidgetItem* SystemMenu::getOrCreateGroup(const QString& id, Roles role, QTreeWidgetItem* parent)
{
  if (auto* existing = getItemById(id))
    return existing;

  auto* group = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(this);

  switch (role)
  {
    case Roles::SubTasks:
      populateItem(group, QIcon(":/icons/structure.svg"), tr("Sub-tasks"), "", id, Roles::SubTasks);
      break;

    case Roles::Capabilities:
      populateItem(group, QIcon(":/icons/structure.svg"), tr("Capabilities"), "", id, Roles::Capabilities);
      break;

    case Roles::Flows:
      populateItem(group, QIcon(":/icons/behaviour.svg"), tr("Flows"), "", id, Roles::Flows);
      break;

    case Roles::PipelineRoot:
      populateItem(group, QIcon(":/icons/pipeline.svg"), tr("Pipelines"), "", id, Roles::PipelineRoot);
      break;

    default:
      assert(false && "Invalid role for group");
      delete group;
      return nullptr;
  }

  return group;
}

QTreeWidgetItem* SystemMenu::getOrCreateChildGroup(const QString& parentId, Roles role)
{
  auto* parent = findParentItemByRole(parentId, role);
  if (!parent)
    return nullptr;

  if (parent->data(TYPE_DATA, Qt::UserRole) == role)
    return parent;

  QString groupId;

  switch (role)
  {
    case Roles::SubTasks:
      groupId = parentId + "::subtasks";
      break;

    case Roles::Capabilities:
      groupId = parentId + "::capabilities";
      break;

    case Roles::Flows:
      groupId = parentId + "::flows";
      break;

    default:
      assert(false && "Invalid role for child group");
      return nullptr;
  }

  return getOrCreateGroup(groupId, role, parent);
}

QTreeWidgetItem* SystemMenu::getItemById(const QString& id) const
{
  for (QTreeWidgetItemIterator it(const_cast<SystemMenu*>(this)); *it; ++it)
    if ((*it)->data(ID_DATA, Qt::UserRole).toString() == id)
      return *it;

  return nullptr;
}

QTreeWidgetItem* SystemMenu::findParentItemByRole(const QString& id, Roles role) const
{
  auto parent = getItemById(id);
  if (!parent)
    return nullptr;

  for (int i = 0; i < parent->childCount(); ++i)
  {
    auto* child = parent->child(i);
    if (child->data(TYPE_DATA, Qt::UserRole) == role)
      return child;
  }

  return parent;
}

void SystemMenu::removeGroupIfEmpty(QTreeWidgetItem* group)
{
  if (!group)
    return;

  if (group->childCount() > 0)
    return;

  // Only remove organizational groups, not normal nodes.
  const auto role = static_cast<Roles>(group->data(TYPE_DATA, Qt::UserRole).toInt());
  switch (role)
  {
    case Roles::SubTasks:
    case Roles::Capabilities:
    case Roles::Flows:
    case Roles::PipelineRoot:
      break;

    default:
      return;
  }

  if (auto* parent = group->parent())
  {
    parent->removeChild(group);
    delete group;
    return;
  }

  const int index = indexOfTopLevelItem(group);
  if (index >= 0)
  {
    takeTopLevelItem(index);
    delete group;
  }
}

void SystemMenu::showContextMenu(const QPoint& pos)
{
  QTreeWidgetItem* selectedItem = itemAt(pos);
  if (!selectedItem)
    return;

  QMenu contextMenu(this);
  contextMenu.setFixedWidth(Config::MENU_WIDTH);
  if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::NodeRole)
  {
    contextMenu.addAction(iconFromTheme("search"), tr("Focus"), this, [this, selectedItem]() {
      emit nodeFocused(selectedItem->data(CANVAS_DATA, Qt::UserRole).toString(), selectedItem->data(ID_DATA, Qt::UserRole).toString());
    });
    contextMenu.addAction(iconFromTheme("edit-delete"), tr("Delete"), this, [selectedItem]() {
      // Update the model and let the events handle the rest
      const auto nodeId = selectedItem->data(ID_DATA, Qt::UserRole).toString();
      SaveInfo::instance().removeNode(nodeId);
    });
  }
  else if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::FlowRole)
  {
    contextMenu.addAction(iconFromTheme("document-edit"), tr("Edit"), this, [this, selectedItem]() { editFlow(selectedItem); });
    contextMenu.addAction(iconFromTheme("edit-delete"), tr("Delete"), this, [this, selectedItem]() { removeFlow(selectedItem); });
  }
  else if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::Pipeline)
  {
    contextMenu.addAction(iconFromTheme("document-edit"), tr("Edit"), this,
                          [this, selectedItem]() { emit editPipeline(selectedItem->data(ID_DATA, Qt::UserRole).toString()); });
    contextMenu.addAction(iconFromTheme("edit-delete"), tr("Delete"), this,
                          [this, selectedItem]() { emit removePipeline(selectedItem->data(ID_DATA, Qt::UserRole).toString()); });
  }
  else
  {
    return;
  }

  contextMenu.exec(viewport()->mapToGlobal(pos));
}

void SystemMenu::onItemClicked(QTreeWidgetItem* item, int /* column */)
{
  if (!item)
    return;

  if (item->data(TYPE_DATA, Qt::UserRole) == Roles::FlowRole)
    editFlow(item);
  else if (item->data(TYPE_DATA, Qt::UserRole) == Roles::NodeRole)
    emit nodeFocused(item->data(CANVAS_DATA, Qt::UserRole).toString(), item->data(ID_DATA, Qt::UserRole).toString());
  else if (item->data(TYPE_DATA, Qt::UserRole) == Roles::Pipeline)
    emit editPipeline(cleanId(item, ID_DATA));
}

void SystemMenu::editFlow(QTreeWidgetItem* item)
{
  auto group = item->parent();
  if (group == nullptr)
  {
    LOG_WARNING("Invalid flow, it has no parent group");
    return;
  }

  auto node = group->parent();
  if (node == nullptr)
  {
    LOG_WARNING("Invalid flow, it has no parent node");
    return;
  }

  auto nodeId = cleanId(node, ID_DATA);
  auto flowId = cleanId(item, ID_DATA);
  emit flowSelected(flowId, nodeId);
}

void SystemMenu::removeFlow(QTreeWidgetItem* item)
{
  auto node = item->parent();
  if (node == nullptr)
  {
    LOG_WARNING("Invalid flow, it has no parent");
    return;
  }

  auto nodeId = cleanId(node, ID_DATA);
  auto flowId = cleanId(item, ID_DATA);

  // Update the model and let the events handle the rest
  SaveInfo::instance().removeFlowFromNode(flowId, nodeId);
}

QString SystemMenu::cleanId(const QTreeWidgetItem* item, const int id, const Qt::ItemDataRole role) const
{
  auto data = item->data(id, role).toString();
  LOG_DEBUG("Cleaning: {} -> {}", data, data.section("::", 0, 0));
  return data.section("::", 0, 0);
}

void SystemMenu::resizeEvent(QResizeEvent* event)
{
  QTreeWidget::resizeEvent(event);
  resizeColumns();
}

void SystemMenu::resizeColumns(int changedColumn, int requestedWidth)
{
  if (mAdjustingColumns.Value())
    return;

  const int totalWidth = viewport()->width();
  if (totalWidth <= 0)
    return;

  mAdjustingColumns.SetUnconditionally(true);

  int nameWidth = 0;
  if (totalWidth <= 2 * MIN_COLUMN_WIDTH)
  {
    // Not enough room to respect the minimum for both columns.
    nameWidth = totalWidth / 2;
  }
  else if (changedColumn == NAME_COLUMN)
  {
    nameWidth = std::clamp(requestedWidth, MIN_COLUMN_WIDTH, totalWidth - MIN_COLUMN_WIDTH);

    mNameColumnRatio = static_cast<qreal>(nameWidth) / totalWidth;
  }
  else if (changedColumn == TYPE_COLUMN)
  {
    const int typeWidth = std::clamp(requestedWidth, MIN_COLUMN_WIDTH, totalWidth - MIN_COLUMN_WIDTH);
    nameWidth = totalWidth - typeWidth;
    mNameColumnRatio = static_cast<qreal>(nameWidth) / totalWidth;
  }
  else
  {
    // Parent widget was resized: preserve the user's chosen ratio.
    nameWidth = static_cast<int>(totalWidth * mNameColumnRatio);
    nameWidth = std::clamp(nameWidth, MIN_COLUMN_WIDTH, totalWidth - MIN_COLUMN_WIDTH);
  }

  header()->resizeSection(NAME_COLUMN, nameWidth);
  header()->resizeSection(TYPE_COLUMN, totalWidth - nameWidth);

  mAdjustingColumns.SetUnconditionally(false);
}
