#pragma once

#include <QDir>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <optional>

#include "pipeline_artifact.h"

namespace maki
{
class PipelineContext
{
public:
  QDir projectDir;
  QDir buildDir;
  QDir generatedDir;

  QVariantMap variables;

  bool hasType(const QString& type) const;

  void addArtifact(const PipelineArtifact& artifact);

  std::optional<PipelineArtifact> artifact(const QString& id) const;
  QMap<QString, PipelineArtifact> artifacts() const;
  QList<PipelineArtifact> artifactsOfType(const QString& type) const;

private:
  QMap<QString, PipelineArtifact> mArtifacts;
};
}  // namespace maki