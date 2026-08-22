#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "ipipeline.h"
#include "pipeline_action.h"
#include "pipeline_artifact.h"
#include "pipeline_context.h"
#include "result.h"

namespace maki
{
class ArduinoPlugin;
}

class ArduinoGenerateCpp : public maki::IPipelineAction
{
public:
  ArduinoGenerateCpp(maki::ArduinoPlugin* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline) override;

private:
  maki::ArduinoPlugin* mGenerator;
};