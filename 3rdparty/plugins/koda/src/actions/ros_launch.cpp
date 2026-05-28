#include "ros_launch.h"

#include <QProcess>
#include <QString>
#include <QVector>

#include "koda_action.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

KodaRosLaunch::KodaRosLaunch(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString KodaRosLaunch::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.launch_ros";
#else
  return "koda.launch_ros";
#endif
}

QString KodaRosLaunch::displayName() const
{
#ifdef USE_ANTLR
  return "Launch ROS";
#else
  return "Launch ROS";
#endif
}

QStringList KodaRosLaunch::consumes() const
{
  return {"ros-project"};
}

QStringList KodaRosLaunch::produces() const
{
  return {""};
}

maki::ResultArtifacts KodaRosLaunch::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running %s", qPrintable(id()));
  const auto artifacts = context.artifactsOfType("ros-project");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"ros-project\"");

  // Generate files
  // If there are multiple artifacts, use the latest one
  const auto project = artifacts.at(artifacts.size() > 1 ? artifacts.size() - 1 : 0);

  auto generated = mGenerator->launchRosProject(project, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  return maki::Artifacts{};
}