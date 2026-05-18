#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "action_parameter.h"
#include "ipipeline.h"
#include "pipeline_artifact.h"
#include "pipeline_context.h"
#include "result.h"

namespace maki
{
class IPipelineAction
{
public:
  virtual ~IPipelineAction() = default;

  virtual QString id() const = 0;
  virtual QString displayName() const = 0;
  virtual QStringList consumes() const = 0;
  virtual QStringList produces() const = 0;

  virtual QVariantMap defaultParameters() const
  {
    return {};
  }

  virtual QVector<ActionParameter> parameters() const
  {
    return {};
  }

  virtual ResultArtifacts run(const PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline) = 0;
};

}  // namespace maki