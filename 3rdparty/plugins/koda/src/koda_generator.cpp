#include "koda_generator.h"

// #include <QApplication>
// #include <QDirIterator>
// #include <QFile>
// #include <QFileInfo>
// #include <QJsonArray>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QLabel>
// #include <QProcess>
// #include <QTextStream>
// #include <algorithm>

#include "actions/koda_action.h"
#include "actions/ros_build.h"
#include "actions/ros_copy.h"
#include "actions/ros_launch.h"
#include "ast/koda_compiler.h"
#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "iui.h"
#include "logging.h"
#include "maki_to_koda.h"
#include "pipeline_artifact.h"
#include "result.h"
#include "string_helpers.h"
#include "types.h"
#include "typing/type_registry.h"

bool KodaGenerator::setup()
{
  if (!mTraceMap)
    mTraceMap = std::make_shared<maki::TraceabilityMap>();

  // Start the ide daemon on a specific port
  return true;
}

bool KodaGenerator::tearDown()
{
  if (auto* tab = mServices->ui())
    tab->deregisterPlugin(languageName());

  return false;
}

void KodaGenerator::setHostServices(maki::IHostServices* services)
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

QList<std::shared_ptr<maki::IPipelineAction>> KodaGenerator::pipelineActions()
{
  return {
    std::make_shared<GenerateKodaAction>(this),
    std::make_shared<KodaRosCopySources>(),
    std::make_shared<KodaRosBuild>(this),
    std::make_shared<KodaRosLaunch>(this),
  };
}

void KodaGenerator::publishErrors(const std::vector<koda::Error>& errors) const
{
  if (!errors.empty())
  {
    auto firstError = errors.front();
    if (mServices)
      mServices->errorOnNode(QString::fromStdString(firstError.nodeId), QString::fromStdString(firstError.flowId), QString::fromStdString(firstError.message));
  }
}

Result<maki::PipelineArtifact> KodaGenerator::generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder)
{
  if (!outputFolder.exists())
    outputFolder.mkdir(".");

  LOG_DEBUG("Generating Koda files with {} nodes", mServices->document()->getnodes().size());
  QString code = "";

  // Clean before generation
  if (mTraceMap)
    mTraceMap->clear();

  const auto* typeRegistry = mServices->document()->getTypesRegistry();
  koda::MakiToKoda makiToKoda(typeRegistry, mTraceMap);
  const auto missionParameters = mServices->document()->getparameters();
  auto generated = makiToKoda.generate(mServices->document()->getnodes(), missionParameters);
  publishErrors(makiToKoda.getErrors());
  if (!generated)
    return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());

  mAST = std::make_shared<koda::System>(makiToKoda.getAST());

  QString fileName = outputFolder.filePath("task.kd");
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return Result<maki::PipelineArtifact>::Failed("Failed to open device for writing: " + fileName.toStdString());

  QTextStream out(&file);
  out << generated.Value();
  file.close();

  LOG_DEBUG("Created file: {}", fileName);

  maki::PipelineArtifact output;
  output.metadata = {
    {"sources", {fileName}},
  };

  return output;
}

Result<maki::PipelineArtifact> KodaGenerator::buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
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

Result<maki::PipelineArtifact> KodaGenerator::launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
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
