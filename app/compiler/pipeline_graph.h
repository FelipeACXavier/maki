#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

#include "plugin_version.h"
#include "result.h"
#include "save_info.h"

class Canvas;

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
  static Result<PipelineGraph> fromFlow(const FlowSaveInfo& info);
  static Result<PipelineGraph> fromCanvas(const Canvas* canvas);

  QJsonObject toJson() const;
};
}  // namespace maki