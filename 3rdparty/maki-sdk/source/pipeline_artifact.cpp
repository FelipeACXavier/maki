#include "pipeline_artifact.h"

namespace maki
{

QJsonObject PipelineArtifact::toJson() const
{
  QJsonObject object;
  object["id"] = id;
  object["type"] = type;
  object["producer"] = producer;
  object["paths"] = QJsonObject::fromVariantMap(paths);
  object["metadata"] = QJsonObject::fromVariantMap(metadata);

  return object;
}

PipelineArtifact PipelineArtifact::fromJson(const QJsonObject& object)
{
  PipelineArtifact artifact;
  if (object.contains("id"))
    artifact.id = object["id"].toString();

  if (object.contains("type"))
    artifact.type = object["type"].toString();

  if (object.contains("paths"))
    artifact.paths = object["paths"].toObject().toVariantMap();

  if (object.contains("producer"))
    artifact.producer = object["producer"].toString();

  if (object.contains("metadata"))
    artifact.metadata = object["metadata"].toObject().toVariantMap();

  return artifact;
}

}  // namespace maki