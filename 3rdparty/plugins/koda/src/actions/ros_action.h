#pragma once

#include "action_parameter.h"
#include "pipeline_action.h"
#include "pipeline_artifact.h"

class GenerateRosAction : public maki::IPipelineAction
{
public:
  GenerateRosAction();
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  QVector<maki::ActionParameter> parameters() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline) override;
};