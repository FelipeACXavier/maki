#include "pipeline_graph.h"

#include <QFile>
#include <QJsonArray>
#include <memory>

#include "elements/transition.h"
#include "logging.h"
#include "node_info.h"
#include "system/canvas.h"
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

  if (object.contains("parameters") && object["parameters"].isArray())
  {
    for (const auto& obj : object["parameters"].toArray())
    {
      auto param = obj.toObject();
      node.parameters[param["key"].toString()] = maki::Value::fromJson(param["value"].toObject());
    }
  }

  return node;
}

void PipelineNode::print() const
{
  LOG_INFO("  Node id: {}", id);
  LOG_INFO("  Node actionId: {}", actionId);
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
  LOG_INFO("  Edge from: {}", from);
  LOG_INFO("  Edge to: {}", to);
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
      RETURN_ON_FAILURE_AS(result, PipelineGraph);
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
    QJsonArray params;
    for (const auto& [key, value] : node.parameters)
    {
      QJsonObject param;
      param["key"] = key;
      param["value"] = value.toJson();
      params.append(param);
    }
    nodeObject["parameters"] = params;
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
    if (const auto* name = node->getProperty("name"))
      pnode.displayName = name->getvalue()->toStringValue();

    for (const auto& property : node->getproperties())
    {
      if (property->getid() == "name")
        continue;

      pnode.parameters[property->getid()] = *dynamic_cast<const maki::Value*>(property->getvalue());
    }

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
      pnode.actionId = toAction(node->nodeType());
      if (const auto* name = node->getProperty("name"))
        pnode.id = name->getvalue()->toStringValue();

      for (const auto& property : node->properties())
        pnode.parameters[property->getid()] = *dynamic_cast<const maki::Value*>(property->getvalue());

      graph.nodes.append(pnode);
    }
    else if (item->type() == TransitionItem::Type)
    {
      auto edge = static_cast<TransitionItem*>(item);
      PipelineEdge pedge;
      if (const auto* name = edge->source()->getProperty("name"))
        pedge.from = name->getvalue()->toStringValue();
      if (const auto* name = edge->destination()->getProperty("name"))
        pedge.to = name->getvalue()->toStringValue();

      graph.edges.append(pedge);
    }
  }

  return graph;
}

void PipelineGraph::print() const
{
  LOG_INFO("Pipeline id: {}", id);
  LOG_INFO("Pipeline name: {}", name);
  LOG_INFO("Pipeline version: {}", version.toString());
  for (const auto& node : nodes)
    node.print();
  for (const auto& edge : edges)
    edge.print();
}
}  // namespace maki