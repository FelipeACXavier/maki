#include "pipeline_context.h"

namespace maki
{

void PipelineContext::addArtifact(const PipelineArtifact& artifact)
{
  mArtifacts.insert(artifact.id, artifact);
}

std::optional<PipelineArtifact> PipelineContext::artifact(const QString& id) const
{
  if (!mArtifacts.contains(id))
    return std::nullopt;

  return mArtifacts.value(id);
}

QList<PipelineArtifact> PipelineContext::artifactsOfType(const QString& type) const
{
  QList<PipelineArtifact> result;

  for (const auto& artifact : mArtifacts)
  {
    if (artifact.type == type)
      result.append(artifact);
  }

  return result;
}

bool PipelineContext::hasType(const QString& type) const
{
  return !artifactsOfType(type).isEmpty();
}

QMap<QString, PipelineArtifact> PipelineContext::artifacts() const
{
  return mArtifacts;
}

}  // namespace maki