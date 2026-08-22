#include "dezyne_action.h"

#include <QString>

#include "../koda_generator.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "types.h"

GenerateDezyneAction::GenerateDezyneAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString GenerateDezyneAction::id() const
{
  return "koda.generate_dezyne";
}

QString GenerateDezyneAction::displayName() const
{
  return "Generate Dezyne";
}

QStringList GenerateDezyneAction::consumes() const
{
  return {"koda"};  // Diagram information
}

QStringList GenerateDezyneAction::produces() const
{
  return {"dezyne"};
}

QVector<maki::ActionParameter> GenerateDezyneAction::parameters() const
{
  return {maki::ActionParameter("Debug", koda::types::TypeReference::createBool(), maki::Value::createBool(false))};
}

maki::ResultArtifacts GenerateDezyneAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());

  const auto artifacts = context.artifactsOfType("koda");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"koda\"");

  const auto koda = artifacts.at(0);
  auto result = mGenerator->generateDezyne(koda, context.buildDir, pipeline);
  if (!result.IsSuccess())
    return maki::ResultArtifacts::Failed(result.ErrorMessage());

  maki::PipelineArtifact artifact = result.Value();
  artifact.id = "dezyne.generated";
  artifact.type = "dezyne";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}