#pragma once

#include "pipeline_action.h"
#include "pipeline_artifact.h"

class KodaGenerator;

class GenerateCppAction : public maki::IPipelineAction
{
public:
  GenerateCppAction(KodaGenerator* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline) override;

private:
  KodaGenerator* mGenerator;
};