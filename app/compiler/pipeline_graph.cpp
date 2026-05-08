#include "pipeline_graph.h"

#include <QFile>
#include <QJsonArray>

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

}  // namespace maki