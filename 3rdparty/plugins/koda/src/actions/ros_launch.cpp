#include "ros_launch.h"

#include <QProcess>
#include <QString>
#include <QVector>

#include "koda_action.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

static const QString ROS_FOLDER = "ROS folder";

KodaRosLaunch::KodaRosLaunch(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString KodaRosLaunch::id() const
{
  return "koda.launch_ros";
}

QString KodaRosLaunch::displayName() const
{
  return "Launch ROS";
}

QStringList KodaRosLaunch::consumes() const
{
  return {"ros-project"};
}

QStringList KodaRosLaunch::produces() const
{
  return {""};
}

QVector<maki::ActionParameter> KodaRosLaunch::parameters() const
{
  const QString home = QDir::homePath();
  return {
      maki::ActionParameter(ROS_FOLDER, koda::types::TypeReference::createString(), maki::Value::createString(home + "/ros2_ws")),
  };
}

maki::ResultArtifacts KodaRosLaunch::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  const auto artifacts = context.artifactsOfType("ros-project");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"ros-project\"");

  if (!parameters.contains(ROS_FOLDER))
    return maki::ResultArtifacts::Failed("No ROS folder provided");

  auto rosFolder = parameters.at(ROS_FOLDER).toString();
  if (!QDir(rosFolder).exists())
    return maki::ResultArtifacts::Failed("ROS folder '{}' does no exist", rosFolder);

  // Generate files
  // If there are multiple artifacts, use the latest one
  const auto project = artifacts.at(artifacts.size() > 1 ? artifacts.size() - 1 : 0);

  auto generated = mGenerator->launchRosProject(project, context.buildDir, rosFolder, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  return maki::Artifacts{};
}