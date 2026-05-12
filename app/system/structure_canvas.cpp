#include "structure_canvas.h"

#include "system/config_table.h"
#include "document/node_info.h"
#include "elements/node.h"

StructureCanvas::StructureCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, storage, configTable, parent)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}

void StructureCanvas::updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding)
{
  Q_UNUSED(storage);

  if (!node || !adding)
    return;

  NodeItem* parent = node->parentNode();
  if (parent && parent->isTaskContainer())
  {
    if (node->isTaskContainer())
      parent->layoutSubtasks();
    else
      parent->relayoutCapabilitySlots();
  }

  if (node->isTaskContainer())
    node->ensureSubtaskConnector(this);
}

void StructureCanvas::createSubtask(NodeItem* parentTask)
{
  if (!parentTask || !parentTask->isTaskContainer())
    return;

  auto cfg = configurationTable()->get(QStringLiteral("Koda::Task"));
  if (!cfg)
    return;

  auto info = std::make_shared<NodeSaveInfo>();
  info->setNodeId(QStringLiteral("Koda::Task"));
  info->setSize(QSizeF(cfg->body.width, cfg->body.height));
  info->setScale(getScale());

  const QRectF pr = parentTask->sceneBoundingRect();
  const QPointF center(pr.right() + 80.0, pr.bottom() + 60.0);

  createNode(NodeCreation::Dropping, info, center, parentTask);
}
