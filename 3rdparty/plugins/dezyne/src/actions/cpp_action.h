#pragma once

#include "pipeline_action.h"
#include "pipeline_artifact.h"

namespace maki
{
class DezynePlugin;

class GenerateCppAction : public maki::IPipelineAction
{
public:
  GenerateCppAction(DezynePlugin* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline) override;

private:
  DezynePlugin* mGenerator;
};
}  // namespace maki
