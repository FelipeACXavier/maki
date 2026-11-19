#include "ros_plugin.h"

#include "actions/ros_build.h"
#include "actions/ros_copy.h"
#include "actions/ros_launch.h"
#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "iui.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"
#include "string_helpers.h"
#include "types.h"
#include "typing/type_registry.h"

namespace maki
{
bool ROSPlugin::setup()
{
  return true;
}

bool ROSPlugin::tearDown()
{
  return true;
}

void ROSPlugin::setHostServices(maki::IHostServices* services)
{
  mServices = services;

  if (auto logger = mServices->logger())
  {
    logging::gSourceName = languageName().toStdString();
    logging::gSilentLog = false;
    logger->registerPlugin(languageName(), logging::gLogToStream);
  }

  buildSettings();
}

QList<std::shared_ptr<maki::IPipelineAction>> ROSPlugin::pipelineActions()
{
  return {
    std::make_shared<RosCopySources>(),
    std::make_shared<RosBuild>(this),
    std::make_shared<RosLaunch>(this),
  };
}

Result<maki::PipelineArtifact> ROSPlugin::buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
                                                          maki::IPipeline* pipeline)
{
  if (!artifact.paths.contains("rootDir"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing root folder");

  const QString projectDir = artifact.paths["rootDir"].toString();
  if (rosFolder.isEmpty())
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing ros folder");

  LOG_DEBUG("Building ROS project in {} with ROS workspace: {}", projectDir, rosFolder);
  pipeline->startGroup("Build ROS");
  QProcess* generate = new QProcess(this);
  generate->setWorkingDirectory(projectDir);
  generate->setProgram("docker");
  // clang-format off
  QStringList args = {
    "run",
    "--rm",
    "-u", "felaze",
    "-e", "TERM=xterm-256color",
    "-e", "CLICOLOR_FORCE=1",
    "-v", rosFolder + ":" + rosFolder,
    "-v", projectDir + ":" + rosFolder + "/src/koda_ros",
    "-w", rosFolder,
    "ros2:v1.0.0",
    "bash", "-ic", "source /opt/ros/humble/setup.bash && colcon build --symlink-install",
  };
  // clang-format on
  generate->setArguments(args);

  pipeline->add(generate, maki::OnFail::STOP);
  pipeline->endGroup();

  return maki::PipelineArtifact{};
}

Result<maki::PipelineArtifact> ROSPlugin::launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
                                                           maki::IPipeline* pipeline)
{
  if (!artifact.paths.contains("rootDir"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing root folder");
  if (!artifact.metadata.contains("packageName"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing package name");
  if (!artifact.metadata.contains("launchFile"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing launch file");

  const QString projectDir = artifact.paths["rootDir"].toString();
  const QString packageName = artifact.metadata["packageName"].toString();
  const QString launchFile = artifact.metadata["launchFile"].toString();

  pipeline->startGroup("Launch ROS");
  QProcess* generate = new QProcess(this);
  generate->setWorkingDirectory(projectDir);
  generate->setProgram("docker");
  // clang-format off
  QStringList args = {
    "run",
    "--rm",
    "--name", "maki_koda_runtime",
    "-u", "felaze",
    "-e", "DISPLAY=" + qEnvironmentVariable("DISPLAY"),
    "-e", "QT_X11_NO_MITSHM=1",
    "-e", "TURTLEBOT3_MODEL=waffle_pi",
    "-e", "GAZEBO_MODEL_PATH=/usr/share/gazebo-11/models:/home/felaze/ros2_ws/world/",
    "--gpus", "all",
    "-e", "NVIDIA_DRIVER_CAPABILITIES=compute,utility,graphics,display",
    "-e", "__NV_PRIME_RENDER_OFFLOAD=1",
    "-e", "__GLX_VENDOR_LIBRARY_NAME=nvidia",
    "-v", "/tmp/.X11-unix:/tmp/.X11-unix:rw",
    "-v", "/home/felaze/ros2_ws:/home/felaze/ros2_ws:rw",
    "-v", projectDir + ":/home/felaze/ros2_ws/src/koda_ros:rw",
    "-w", "/home/felaze/ros2_ws",
    "ros2:v1.0.0",
    "bash", "-ic", QString("source /opt/ros/humble/setup.bash && source install/setup.bash && ros2 launch %1 %2 2>&1 | grep --line-buffered -v \"Sensor origin\" | grep --line-buffered -v rviz | grep --line-buffered -v \"frame does not exist\"").arg(packageName, launchFile),
  };
  // clang-format on
  generate->setArguments(args);

  pipeline->add(generate, maki::OnFail::EXECUTE, [this](int& exitCode, QProcess::ExitStatus& status) {
    LOG_DEBUG("Stopping docker container");

    auto* stop = new QProcess(this);
    connect(stop, &QProcess::finished, this, [this, stop, &exitCode, &status](int code, QProcess::ExitStatus) {
      stop->deleteLater();
      if (code == 0)
      {
        exitCode = 0;
        status = QProcess::NormalExit;
        return;
      }

      LOG_DEBUG("Failed to stop cleanly, killing docker container");
      auto* kill = new QProcess(this);
      connect(kill, &QProcess::finished, this, [kill, &exitCode, &status](int code, QProcess::ExitStatus) {
        exitCode = code;
        if (code == 0)
          status = QProcess::NormalExit;

        kill->deleteLater();
      });
      kill->start("docker", {"kill", "maki_koda_runtime"});
    });
    stop->start("docker", {"stop", "-t", "5", "maki_koda_runtime"});
  });
  pipeline->endGroup();

  return maki::PipelineArtifact{};
}

void ROSPlugin::buildSettings()
{
}

}  // namespace maki
