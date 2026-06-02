#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "ipipeline.h"
#include "koda_generator.h"
#include "pipeline_action.h"
#include "pipeline_artifact.h"
#include "pipeline_context.h"

class KodaGenerator;

class KodaRosBuild : public maki::IPipelineAction
{
public:
  KodaRosBuild(KodaGenerator* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  maki::ResultArtifacts run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline) override;

private:
  KodaGenerator* mGenerator;
};