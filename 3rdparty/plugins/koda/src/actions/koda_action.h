#pragma once

#include "pipeline_action.h"
#include "pipeline_artifact.h"

class KodaGenerator;

class GenerateKodaAction : public maki::IPipelineAction
{
public:
  GenerateKodaAction(KodaGenerator* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline) override;

private:
  KodaGenerator* mGenerator;
};