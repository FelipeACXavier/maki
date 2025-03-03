#include "generator.h"

#include "logging.h"
#include "view/connection.h"
#include "view/connector.h"
#include "view/node.h"

Generator::Generator()
{
}

VoidResult Generator::generate(Canvas* canvas)
{
  if (!canvas)
    return VoidResult::Failed("No canvas provided");

  LOG_INFO("======================================");
  LOG_INFO("Starting generation");
  // Main generation loop, we need to:
  // For each component:
  //    1. Find all starting points
  //    2. Define the functions
  //    3. Write the computations
  //    4. Connect the callbacks
  for (const auto& item : canvas->items())
  {
    RETURN_ON_FAILURE(generateFlow(item, canvas));
  }

  return VoidResult();
}

VoidResult Generator::generateFlow(const QGraphicsItem* item, Canvas* canvas)
{
  if (item->type() != NodeItem::Type)
    return VoidResult();

  auto node = static_cast<const NodeItem*>(item);
  if (node->nodeType() != "Start")
    return VoidResult();

  return generateNode(item, canvas);
}

VoidResult Generator::generateNode(const QGraphicsItem* item, Canvas* canvas)
{
  if (item->type() != NodeItem::Type)
    return VoidResult();

  auto node = static_cast<const NodeItem*>(item);
  LOG_DEBUG("Looping through: %s", qPrintable(node->nodeType()));

  // Here, we need to parse the behaviour in order to know how to handle the connectors
  auto behaviour = node->behaviour();

  LOG_DEBUG("Item with behaviour: %s", qPrintable(behaviour));

  // Each connection is a call
  for (const auto& connector : node->connectors())
  {
    if (connector->connectorType() != Types::ConnectorType::OUT)
      continue;

    auto id = connector->connectorId();
    auto toReplace = QStringLiteral("$connectors.%1").arg(id);
    auto callName = QStringLiteral("connection_%1()").arg(id);

    LOG_INFO("Replacing %s in the behaviour", qPrintable(toReplace));
    behaviour.replace(toReplace, callName);

    // Continue through the three
    RETURN_ON_FAILURE(generateConnector(connector, canvas));
  }

  LOG_DEBUG("Updated behaviour: %s", qPrintable(behaviour));

  return VoidResult();
}

VoidResult Generator::generateConnector(const std::shared_ptr<Connector> item, Canvas* canvas)
{
  if (item->type() != Connector::Type)
    return VoidResult::Failed("Passed object is not a connector");

  LOG_DEBUG("Looping connector: %s", qPrintable(item->connectorId()));
  for (const auto& connection : item->connections())
  {
    auto destination = connection->destination();
    if (!destination)
      continue;

    // The parent of the connection is the NodeItem
    RETURN_ON_FAILURE(generateNode(destination->parentItem(), canvas));
  }

  return VoidResult();
}
