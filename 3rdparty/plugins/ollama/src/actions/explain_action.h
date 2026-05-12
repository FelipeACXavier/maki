#pragma once

#include "pipeline_action.h"
#include "pipeline_artifact.h"

namespace maki
{
class OllamaPlugin;

class OllamaExplainAction : public maki::IPipelineAction
{
public:
  OllamaExplainAction(OllamaPlugin* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline) override;

private:
  OllamaPlugin* mGenerator;
};
}  // namespace maki
