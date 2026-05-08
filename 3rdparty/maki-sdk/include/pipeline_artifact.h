#pragma once

#include <QJsonObject>
#include <QString>

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
}  // namespace maki