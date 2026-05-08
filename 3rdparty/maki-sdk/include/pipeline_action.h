#pragma once

#include <QString>
#include <QStringList>
#include <QVariantMap>

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

  virtual VoidResult run(PipelineContext& context, const QVariantMap& parameters) = 0;
};
}  // namespace maki