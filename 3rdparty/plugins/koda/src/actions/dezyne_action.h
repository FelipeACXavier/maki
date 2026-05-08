#pragma once

#include "pipeline_action.h"

class KodaGenerator;

class GenerateDezyneAction final : public maki::IPipelineAction
{
public:
  GenerateDezyneAction(KodaGenerator* generator);
  QString id() const override;
  QString displayName() const override;
  QStringList consumes() const override;
  QStringList produces() const override;
  VoidResult run(maki::PipelineContext& context, const QVariantMap& parameters) override;

private:
  KodaGenerator* mGenerator;
};