#include "dezyne_action.h"

#include <QString>

#include "../koda_generator.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "types.h"

static const QString SIMULATE_KEY = "Simulate";
static const QString START_WAIT = "Start wait";

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
  return {"dezyne", "ros-project"};
}

QVector<maki::ActionParameter> GenerateDezyneAction::parameters() const
{
  return {
      maki::ActionParameter("Debug", koda::types::TypeReference::createBool(), maki::Value::createBool(false)),
      maki::ActionParameter(SIMULATE_KEY, koda::types::TypeReference::createBool(), maki::Value::createBool(true)),
      maki::ActionParameter(START_WAIT, koda::types::TypeReference::createReal(), maki::Value::createReal(10)),
  };
}

maki::ResultArtifacts GenerateDezyneAction::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());

  const auto artifacts = context.artifactsOfType("koda");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"koda\"");

  const auto koda = artifacts.at(0);
  auto result = mGenerator->generateDezyne(koda, context.buildDir, parameters, pipeline);
  if (!result.IsSuccess())
    return maki::ResultArtifacts::Failed(result.ErrorMessage());

  maki::PipelineArtifact artifact = result.Value();
  artifact.id = "dezyne.generated";
  artifact.type = "dezyne";
  artifact.producer = id();

  auto modelsOutputFolder = QDir(context.buildDir.absolutePath() + "/ros");

  maki::PipelineArtifact rosArtifact = result.Value();
  rosArtifact.id = "dezyne.generate_ros";
  rosArtifact.type = "ros-project";
  rosArtifact.producer = id();
  rosArtifact.paths = {
      {"rootDir", modelsOutputFolder.absolutePath()},
      {"includeDir", modelsOutputFolder.absolutePath() + "/include"},
      {"sourceDir", modelsOutputFolder.absolutePath() + "/src"},
      {"launchDir", modelsOutputFolder.absolutePath() + "/launch"},
      {"configDir", modelsOutputFolder.absolutePath() + "/config"},
  };
  rosArtifact.metadata = {
      {"package", modelsOutputFolder.absolutePath() + "/package.xml"},
      {"cmake", modelsOutputFolder.absolutePath() + "/CMakeLists.txt"},
      {"packageName", QString::fromStdString("koda_ros")},
      {"launchFile", QString::fromStdString("koda.launch.py")},
  };

  return maki::Artifacts{artifact, rosArtifact};
}