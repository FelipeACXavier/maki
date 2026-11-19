#include "ros_launch.h"

#include <QProcess>
#include <QString>
#include <QVector>

#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"
#include "ros_plugin.h"

namespace maki
{
static const QString ROS_FOLDER = "ROS folder";

RosLaunch::RosLaunch(ROSPlugin* generator)
    : mGenerator(generator)
{
}

QString RosLaunch::id() const
{
  return "koda.launch_ros";
}

QString RosLaunch::displayName() const
{
  return "Launch ROS";
}

QStringList RosLaunch::consumes() const
{
  return {"ros-project"};
}

QStringList RosLaunch::produces() const
{
  return {""};
}

QVector<maki::ActionParameter> RosLaunch::parameters() const
{
  const QString home = QDir::homePath();
  return {
    maki::ActionParameter(ROS_FOLDER, koda::types::TypeReference::createString(), maki::Value::createString(home + "/ros2_ws")),
  };
}

maki::ResultArtifacts RosLaunch::run(const maki::PipelineContext& context, const maki::ValueMap& parameters, maki::IPipeline* pipeline)
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
}  // namespace maki
