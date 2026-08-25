#include "ros_build.h"

#include <QProcess>
#include <QString>
#include <QVector>

#include "koda_action.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

KodaRosBuild::KodaRosBuild(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString KodaRosBuild::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.build_ros";
#else
  return "koda.build_ros";
#endif
}

QString KodaRosBuild::displayName() const
{
#ifdef USE_ANTLR
  return "Build ROS";
#else
  return "Build ROS";
#endif
}

QStringList KodaRosBuild::consumes() const
{
  return {"ros-project"};
}

QStringList KodaRosBuild::produces() const
{
  return {"ros-executable"};
}

maki::ResultArtifacts KodaRosBuild::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  const auto artifacts = context.artifactsOfType("ros-project");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"ros-project\"");

  // Generate files
  // If there are multiple artifacts, use the latest one
  const auto project = artifacts.at(artifacts.size() > 1 ? artifacts.size() - 1 : 0);

  auto generated = mGenerator->buildRosProject(project, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  maki::PipelineArtifact artifact = generated.Value();
  artifact.id = "koda.build_ros";
  artifact.type = "ros-executable";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}