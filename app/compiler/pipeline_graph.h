#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

#include "plugin_version.h"
#include "result.h"

namespace maki
{
struct PipelineNode
{
  QString id;
  QString actionId;
  QVariantMap parameters;

  static Result<PipelineNode> fromJson(const QJsonObject& object);
};

struct PipelineEdge
{
  QString from;
  QString to;

  static Result<PipelineEdge> fromJson(const QJsonObject& object);
};

struct PipelineGraph
{
  QString id;
  QString name;
  PluginVersion version;

  QList<PipelineNode> nodes;
  QList<PipelineEdge> edges;

  static Result<PipelineGraph> fromJson(const QJsonObject& object);
  static Result<PipelineGraph> fromJsonFile(const QString& filename);

  QJsonObject toJson() const;
};
}  // namespace maki