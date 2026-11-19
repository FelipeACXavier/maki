#include "pipeline_context.h"

// I had to add and remove these so many times that I decided to leave them here
// #include <QJsonDocument>
// #include "logging.h"

namespace maki
{

void PipelineContext::addArtifact(const PipelineArtifact& artifact)
{
  // const auto json = QJsonDocument(artifact.toJson()).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Adding artifact:\n{}", QString(json));
  mArtifacts.insert(artifact.id, artifact);
}

void PipelineContext::addPendingArtifact(const PipelineArtifact& artifact)
{
  mPending.push_back(artifact);
}

void PipelineContext::commitPendingArtifact()
{
  if (mPending.isEmpty())
    return;

  for (const auto& artifact : mPending)
    addArtifact(artifact);

  mPending.clear();
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
    if (artifact.type == type)
      result.append(artifact);

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
