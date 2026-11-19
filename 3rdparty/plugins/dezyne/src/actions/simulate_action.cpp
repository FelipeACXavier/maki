#include "simulate_action.h"

#include <QString>
#include <QVector>

#include "dezyne_plugin.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

namespace maki
{
DezyneSimulateAction::DezyneSimulateAction(DezynePlugin* generator)
    : mGenerator(generator)
{
}

QString DezyneSimulateAction::id() const
{
  return "koda.simulate";
}

QString DezyneSimulateAction::displayName() const
{
  return "Simulate Dezyne";
}

QStringList DezyneSimulateAction::consumes() const
{
  return {"dezyne.source"};
}

QStringList DezyneSimulateAction::produces() const
{
  return {};
}

maki::ResultArtifacts DezyneSimulateAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());

  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("dezyne");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"dezyne\"");

  // Generate files
  const auto dezyne = artifacts.at(0);
  auto output = mGenerator->simulate(dezyne);
  if (!output.IsSuccess())
    return maki::ResultArtifacts::Failed(output.ErrorMessage());

  return maki::Artifacts{};
}
}  // namespace maki
