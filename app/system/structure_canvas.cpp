#include "structure_canvas.h"

#include <memory>

#include "elements/flow.h"
#include "logging.h"
#include "node_info.h"

StructureCanvas::StructureCanvas(std::shared_ptr<SaveInfo> storage, const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(canvasId, configTable, router, parent)
    , mStorage(storage)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}

void StructureCanvas::updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding)
{
  if (!adding)
    mStorage->removeNode(node->id());
}

void StructureCanvas::addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (node->parentNode() == nullptr)
  {
    // Adding task
    mStorage->addNode(info);
  }

  Canvas::addedItemNode(node, info);
}

void StructureCanvas::onCleanChanged(bool state)
{
  emit cleanChanged(id(), id(), state);
}
