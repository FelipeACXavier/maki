#include "pipeline_graph.h"

#include <QFile>
#include <QJsonArray>
#include <memory>

#include "canvas.h"
#include "logging.h"
#include "node_info.h"
#include "transition.h"
#include "transition_info.h"

namespace maki
{
Result<PipelineNode> PipelineNode::fromJson(const QJsonObject& object)
{
  if (!object.contains("id"))
    return Result<PipelineNode>::Failed("Missing node id");
  else if (!object.contains("actionId"))
    return Result<PipelineNode>::Failed("Missing node actionId");

  PipelineNode node;
  node.id = object["id"].toString();
  node.actionId = object["actionId"].toString();

  if (object.contains("parameters"))
    node.parameters = object["parameters"].toObject().toVariantMap();

  return node;
}

void PipelineNode::print() const
{
  LOG_INFO("  Node id: %s", qPrintable(id));
  LOG_INFO("  Node actionId: %s", qPrintable(actionId));
}

Result<PipelineEdge> PipelineEdge::fromJson(const QJsonObject& object)
{
  if (!object.contains("from"))
    return Result<PipelineEdge>::Failed("Missing edge from");
  else if (!object.contains("to"))
    return Result<PipelineEdge>::Failed("Missing edge to");

  PipelineEdge edge;
  edge.from = object["from"].toString();
  edge.to = object["to"].toString();

  return edge;
}

void PipelineEdge::print() const
{
  LOG_INFO("  Edge from: %s", qPrintable(from));
  LOG_INFO("  Edge to: %s", qPrintable(to));
}

Result<PipelineGraph> PipelineGraph::fromJson(const QJsonObject& object)
{
  if (!object.contains("id"))
    return Result<PipelineGraph>::Failed("Missing graph id");
  else if (!object.contains("version"))
    return Result<PipelineGraph>::Failed("Missing graph version");
  else if (!object.contains("name"))
    return Result<PipelineGraph>::Failed("Missing graph name");

  PipelineGraph graph;
  graph.id = object["id"].toString();
  graph.name = object["name"].toString();
  graph.version = PluginVersion::fromString(object["version"].toString());

  if (object.contains("nodes"))
  {
    for (const auto& node : object["nodes"].toArray())
    {
      auto result = PipelineNode::fromJson(node.toObject());
      if (!result)
        return Result<PipelineGraph>::Failed(result.ErrorMessage());

      graph.nodes.push_back(result.Value());
    }
  }

  if (object.contains("edges"))
  {
    for (const auto& edge : object["edges"].toArray())
    {
      auto result = PipelineEdge::fromJson(edge.toObject());
      if (!result)
        return Result<PipelineGraph>::Failed(result.ErrorMessage());

      graph.edges.push_back(result.Value());
    }
  }
  return graph;
}

Result<PipelineGraph> PipelineGraph::fromJsonFile(const QString& filename)
{
  QFile configFile(filename);

  if (!configFile.open(QFile::ReadOnly))
    return Result<PipelineGraph>::Failed("Failed to open graph file");

  QJsonDocument document = QJsonDocument::fromJson(configFile.readAll());
  configFile.close();

  return PipelineGraph::fromJson(document.object());
}

QJsonObject PipelineGraph::toJson() const
{
  QJsonObject object;

  object["version"] = version.toString();
  object["id"] = id;
  object["name"] = name;

  QJsonArray nodeArray;
  for (const auto& node : nodes)
  {
    QJsonObject nodeObject;
    nodeObject["id"] = node.id;
    nodeObject["action"] = node.actionId;
    nodeObject["parameters"] = QJsonObject::fromVariantMap(node.parameters);

    nodeArray.append(nodeObject);
  }

  object["nodes"] = nodeArray;

  QJsonArray edgeArray;
  for (const auto& edge : edges)
  {
    QJsonObject edgeObject;
    edgeObject["from"] = edge.from;
    edgeObject["to"] = edge.to;

    edgeArray.append(edgeObject);
  }

  object["edges"] = edgeArray;

  return object;
}

Result<PipelineGraph> PipelineGraph::fromFlow(const FlowSaveInfo& info)
{
  PipelineGraph graph;
  auto toAction = [](const QString& n) {
    auto index = n.lastIndexOf(':') + 1;
    return n.mid(index, n.size() - index);
  };

  for (const auto& inode : info.getnodes())
  {
    auto node = std::dynamic_pointer_cast<NodeSaveInfo>(inode);
    PipelineNode pnode;
    pnode.id = node->getid();
    pnode.actionId = toAction(node->getnodeId());
    pnode.displayName = node->getProperty("name").toString();
    QVariantMap params;
    for (const auto& key : node->getproperties().keys())
    {
      if (key == "name")
        continue;

      params[key] = node->getproperties()[key];
    }

    pnode.parameters = params;

    graph.nodes.append(pnode);
  }

  for (const auto& it : info.gettransitions())
  {
    auto t = std::dynamic_pointer_cast<TransitionSaveInfo>(it);
    PipelineEdge edge;
    edge.from = t->getsrcId();
    edge.to = t->getdstId();
    graph.edges.append(edge);
  }

  return graph;
}

Result<PipelineGraph> PipelineGraph::fromCanvas(const Canvas* canvas)
{
  PipelineGraph graph;
  auto toAction = [](const QString& n) {
    auto index = n.lastIndexOf(':') + 1;
    return n.mid(index, n.size() - index);
  };

  for (const auto& item : canvas->items())
  {
    if (item->type() == NodeItem::Type)
    {
      auto node = static_cast<NodeItem*>(item);
      PipelineNode pnode;
      pnode.id = node->getProperty("name").toString();
      pnode.actionId = toAction(node->nodeType());
      pnode.parameters = node->saveInfo().getproperties();

      graph.nodes.append(pnode);
    }
    else if (item->type() == TransitionItem::Type)
    {
      auto edge = static_cast<TransitionItem*>(item);
      PipelineEdge pedge;
      pedge.from = edge->source()->getProperty("name").toString();
      pedge.to = edge->destination()->getProperty("name").toString();
      graph.edges.append(pedge);
    }
  }

  return graph;
}

void PipelineGraph::print() const
{
  LOG_INFO("Pipeline id: %s", qPrintable(id));
  LOG_INFO("Pipeline name: %s", qPrintable(name));
  LOG_INFO("Pipeline version: %s", qPrintable(version.toString()));
  for (const auto& node : nodes)
    node.print();
  for (const auto& edge : edges)
    edge.print();
}
}  // namespace maki