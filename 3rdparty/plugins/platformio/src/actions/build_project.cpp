#include "build_project.h"

#include "../platformio.h"
#include "logging.h"
#include "pipeline_artifact.h"

PlatformIOBuildAction::PlatformIOBuildAction(maki::PlatformIOPlugin* generator)
    : mGenerator(generator)
{
}

QString PlatformIOBuildAction::id() const
{
  return "platformio.build_project";
}

QString PlatformIOBuildAction::displayName() const
{
  return "Build Platform IO project";
}

QStringList PlatformIOBuildAction::consumes() const
{
  return {"platformio-project"};
}

QStringList PlatformIOBuildAction::produces() const
{
  return {"firmware-image"};
}

maki::ResultArtifacts PlatformIOBuildAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  const auto artifacts = context.artifactsOfType("platformio-project");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"platformio-project\"");

  // Generate files
  // If there are multiple artifacts, use the latest one
  const auto project = artifacts.at(artifacts.size() > 1 ? artifacts.size() - 1 : 0);

  auto generated = mGenerator->buildProject(project, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  maki::PipelineArtifact artifact = generated.Value();
  artifact.id = "platformio.build_project";
  artifact.type = "firmware-image";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}