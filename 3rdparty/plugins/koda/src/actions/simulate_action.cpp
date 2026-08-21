#include "simulate_action.h"

#include <QString>
#include <QVector>

#include "../koda_generator.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

KodaSimulateAction::KodaSimulateAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString KodaSimulateAction::id() const
{
  return "koda.simulate";
}

QString KodaSimulateAction::displayName() const
{
  return "Simulate Dezyne";
}

QStringList KodaSimulateAction::consumes() const
{
  return {"dezyne"};
}

QStringList KodaSimulateAction::produces() const
{
  return {};
}

maki::ResultArtifacts KodaSimulateAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
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