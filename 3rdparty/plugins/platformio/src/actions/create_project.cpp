#include "create_project.h"

#include "../platformio.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

PlatformIOCreateProject::PlatformIOCreateProject(maki::PlatformIOPlugin* generator)
    : mGenerator(generator)
{
}

QString PlatformIOCreateProject::id() const
{
  return "platformio.create_project";
}

QString PlatformIOCreateProject::displayName() const
{
  return "Create Platform IO project";
}

QStringList PlatformIOCreateProject::consumes() const
{
  return {};
}

QStringList PlatformIOCreateProject::produces() const
{
  return {"platformio-project"};
}

maki::ResultArtifacts PlatformIOCreateProject::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running %s", qPrintable(id()));

  QDir projectFolder = QDir(context.buildDir.absolutePath() + "/platformio");
  auto generated = mGenerator->initialiseProject(projectFolder, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  maki::PipelineArtifact artifact = generated.Value();
  artifact.id = "platformio.create_project";
  artifact.type = "platformio-project";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}