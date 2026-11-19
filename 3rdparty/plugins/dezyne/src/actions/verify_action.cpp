#include "verify_action.h"

#include <QString>
#include <QVector>

#include "dezyne_plugin.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

namespace maki
{
DezyneVerifyAction::DezyneVerifyAction(DezynePlugin* generator)
    : mGenerator(generator)
{
}

QString DezyneVerifyAction::id() const
{
  return "dezyne.verify";
}

QString DezyneVerifyAction::displayName() const
{
  return "Verify Dezyne";
}

QStringList DezyneVerifyAction::consumes() const
{
  return {"dezyne"};
}

QStringList DezyneVerifyAction::produces() const
{
  return {"dezyne.verified"};
}

maki::ResultArtifacts DezyneVerifyAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("dezyne");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"dezyne\"");

  // Generate files
  const auto dezyne = artifacts.at(0);
  auto verified = mGenerator->verify(dezyne, context.buildDir, pipeline);
  if (!verified)
    return maki::ResultArtifacts::Failed(verified.ErrorMessage());

  return maki::Artifacts();
}
}  // namespace maki
