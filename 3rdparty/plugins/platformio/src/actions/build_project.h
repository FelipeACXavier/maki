#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "pipeline_action.h"
#include "pipeline_context.h"
#include "result.h"

namespace maki
{
class PlatformIOPlugin;
}

class PlatformIOBuildAction : public maki::IPipelineAction
{
public:
  PlatformIOBuildAction(maki::PlatformIOPlugin* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline) override;

private:
  maki::PlatformIOPlugin* mGenerator;
};