#include "cpp_action.h"

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
  return "dezyne.generate_cpp";
}

QString GenerateCppAction::displayName() const
{
  return "Generate Dezyne Cpp";
}

QStringList GenerateCppAction::consumes() const
{
  return {"dezyne.source"};  // Diagram information
}

QStringList GenerateCppAction::produces() const
{
  return {"cpp.source"};
}

maki::ResultArtifacts GenerateCppAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("dezyne.source");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"dezyne\"");

  // Generate files
  const auto dezyne = artifacts.at(0);
  auto generated = mGenerator->generateCpp(dezyne, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  maki::PipelineArtifact artifact = generated.Value();
  artifact.id = "dezyne.generate_dezyne_cpp";
  artifact.type = "cpp";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}
}  // namespace maki
