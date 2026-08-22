#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

#include "mission_parameter.h"
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
  QString displayName;
  maki::ValueMap parameters;

  static Result<PipelineNode> fromJson(const QJsonObject& object);
  void print() const;
};

struct PipelineEdge
{
  QString from;
  QString to;

  static Result<PipelineEdge> fromJson(const QJsonObject& object);
  void print() const;
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
  void print() const;
};
}  // namespace maki