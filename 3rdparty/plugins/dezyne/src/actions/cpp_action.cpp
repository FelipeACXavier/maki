#include "cpp_action.h"

#include "constants.h"
#include "dezyne_plugin.h"
#include "logging.h"
#include "pipeline_artifact.h"

namespace maki
{
GenerateCppAction::GenerateCppAction(DezynePlugin* generator)
    : mGenerator(generator)
{
}

QString GenerateCppAction::id() const
{
  return dezyne::GENERATE_CPP_SOURCE_ACTION_ID;
}

QString GenerateCppAction::displayName() const
{
  return "Generate Dezyne Cpp";
}

QStringList GenerateCppAction::consumes() const
{
  return {dezyne::SOURCE_ARTIFACT_ID};  // Diagram information
}

QStringList GenerateCppAction::produces() const
{
  return {dezyne::CPP_SOURCE_ARTIFACT_ID};
}

maki::ResultArtifacts GenerateCppAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType(dezyne::SOURCE_ARTIFACT_ID);
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"{}\"", dezyne::SOURCE_ARTIFACT_ID);

  // Generate files
  const auto dezyne = artifacts.at(0);
  auto generated = mGenerator->generateCpp(dezyne, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  maki::PipelineArtifact artifact = generated.Value();
  artifact.id = dezyne::GENERATE_CPP_SOURCE_ACTION_ID;
  artifact.type = dezyne::CPP_SOURCE_ARTIFACT_ID;
  artifact.producer = id();

  return maki::Artifacts{artifact};
}
}  // namespace maki
