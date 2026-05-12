#pragma once

#include <QJsonObject>
#include <QString>

#include "result.h"

namespace maki
{
struct PipelineArtifact
{
  QString id;         // e.g. "dezyne.main"
  QString type;       // e.g. "diagram", "koda", "dezyne", "cpp", "platformio-project"
  QString producer;   // pipeline node id
  QVariantMap paths;  // files or directories
  QVariantMap metadata;

  QJsonObject toJson() const;
  static PipelineArtifact fromJson(const QJsonObject& object);
};

typedef QList<maki::PipelineArtifact> Artifacts;
typedef Result<Artifacts> ResultArtifacts;
typedef Result<maki::PipelineArtifact> ResultArtifact;

}  // namespace maki