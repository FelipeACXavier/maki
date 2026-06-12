#include "koda_generator.h"

#include <QApplication>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>
#include <chrono>
#include <filesystem>
#include <thread>

#include "actions/cpp_action.h"
#include "actions/dezyne_action.h"
#include "actions/koda_action.h"
#include "actions/ros_action.h"
#include "actions/ros_build.h"
#include "actions/ros_copy.h"
#include "actions/ros_launch.h"
#include "actions/simulate_action.h"
#include "actions/verify_action.h"
#include "dzn_client/dezyne_simulator.h"
#include "dzn_client/simulation_scene.h"
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

#ifdef USE_ANTLR
#include "ast/koda_compiler.h"
#include "maki_to_koda.h"
#endif

#define APPEND_OR_RETURN_ON_FAILURE(v, func) \
  do                                         \
  {                                          \
    auto ret = func;                         \
    if (!ret.IsSuccess())                    \
      return ret;                            \
                                             \
    v += ret.Value();                        \
  } while (0)

VoidResult copyDirectory(const QString& sourceDir, const QString& targetDir)
{
  namespace fs = std::filesystem;

  fs::path libSrcPath = sourceDir.toStdString();
  fs::path libDstPath = targetDir.toStdString();

  std::error_code ec;
  if (!fs::exists(libSrcPath, ec))
    return VoidResult::Failed("Dezyne library source folder does not exist: " + libSrcPath.string());

  // Make sure the files exist
  fs::create_directories(libDstPath, ec);
  if (ec)
    return VoidResult::Failed("Could not create Dezyne library output folder: " + libDstPath.string());

  fs::copy(libSrcPath, libDstPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
  if (ec)
    return VoidResult::Failed("Could not copy Dezyne libraries to output folder: " + ec.message());

  return VoidResult();
}

bool KodaGenerator::setup()
{
  if (!mSimulator)
  {
    mSimulator = new DezyneSimulator(this);

    connect(mSimulator, &DezyneSimulator::simulationStarted, this, &KodaGenerator::simulationStarted);
    connect(mSimulator, &DezyneSimulator::simulationUpdated, this, &KodaGenerator::simulationUpdated);
  }

  // Start the ide daemon on a specific port
  return startDaemon();
  // return true;
}

bool KodaGenerator::tearDown()
{
  if (mDaemon == nullptr)
    return true;

  if (auto* tab = mServices->ui())
    tab->deregisterPlugin(languageName());

  if (mDaemon->state() == QProcess::Running)
  {
    mDaemon->terminate();

    if (!mDaemon->waitForFinished(2500))
    {
      mDaemon->kill();
      mDaemon->waitForFinished();
    }
  }

  mDaemon = nullptr;

  return true;
}

void KodaGenerator::buildSettings()
{
  // -d, --debug             enable debug ouput
  // -b, --view-port=PORT    browser view listen PORT [3002]
  // -e, --editor-port=PORT  editor listen PORT [3003]
  // -h, --help              display this help and exit
  // -i, --ide-port=PORT     ide command listen PORT [3001]
  // -l, --log-file=LOG      write output to LOG
  // -v, --verbose           be more verbose, show progress
  // -w, --http-port=PORT    web server http listen PORT [3000]

  maki::SettingField httpPort;
  httpPort.setKey("httpPort");
  httpPort.setLabel("HTTP Port");
  httpPort.setDescription("Web server HTTP port");
  httpPort.setDefaultValue(3000);

  QVariantMap httpMetadata;
  httpMetadata["min"] = 3000;
  httpMetadata["max"] = 4000;
  httpPort.setMetadata(httpMetadata);

  httpPort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(httpPort);

  maki::SettingField idePort;
  idePort.setKey("idePort");
  idePort.setLabel("IDE Port");
  idePort.setDescription("IDE command listen port");
  idePort.setDefaultValue(3001);
  idePort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(idePort);

  maki::SettingField viewPort;
  viewPort.setKey("viewPort");
  viewPort.setLabel("View Port");
  viewPort.setDescription("Browser view listen port");
  viewPort.setDefaultValue(3002);
  viewPort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(viewPort);

  maki::SettingField editorPort;
  editorPort.setKey("editorPort");
  editorPort.setLabel("Editor Port");
  editorPort.setDescription("Editor listen port");
  editorPort.setDefaultValue(3003);
  editorPort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(editorPort);

  maki::SettingField debug;
  debug.setKey("debug");
  debug.setLabel("Debug");
  debug.setDescription("Enable debug output");
  debug.setDefaultValue(false);
  debug.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(debug);

  maki::SettingField verbose;
  verbose.setKey("verbose");
  verbose.setLabel("Verbose");
  verbose.setDescription("Be more verbose, show progress");
  verbose.setDefaultValue(false);
  verbose.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(verbose);

  maki::SettingField taskOnly;
  taskOnly.setKey("taskOnly");
  taskOnly.setLabel("Task only");
  taskOnly.setDescription("Only verify the top level task");
  taskOnly.setDefaultValue(false);
  taskOnly.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(taskOnly);

  maki::SettingField validateOnSave;
  validateOnSave.setKey("validateOnSave");
  validateOnSave.setLabel("Validate on save");
  validateOnSave.setDescription("Validate the system on every save");
  validateOnSave.setDefaultValue(false);
  validateOnSave.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(validateOnSave);
}

void KodaGenerator::settingsChanged(const QVector<maki::SettingField>& settings)
{
  for (auto& setting : mSettings)
  {
    for (const auto& incoming : settings)
    {
      if (setting.getKey() != incoming.getKey())
        continue;

      setting.setValue(incoming.getValue());
    }
  }
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

  // Setup settings
  if (auto service = mServices->settings())
  {
    service->registerSettings(languageName(), version(), manifest().iconPath(), mSettings);

    auto settings = service->getPluginSettings(languageName());
    if (!settings.isEmpty())
      mSettings = settings;
  }

  if (auto pluginTab = mServices->ui())
    pluginTab->registerPlugin(languageName(), [this](QGraphicsScene* scene) {
      return createSimulationScene(scene, mLastUpdate);
    });
}

void KodaGenerator::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

VoidResult KodaGenerator::simulate(const maki::PipelineArtifact& artifact)
{
  LOG_INFO("Running simulation");

  if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no services provided");
  else if (mServices->ui() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no plugin tab provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no document provided");
  else if (!setup())
    return VoidResult::Failed("Could not setup simulator");

  if (!artifact.paths.contains("modelDir"))
    return VoidResult::Failed("No model folder provided");
  if (!artifact.metadata.contains("sources"))
    return VoidResult::Failed("No sources provided");
  if (!artifact.metadata.contains("includes"))
    return VoidResult::Failed("No includes provided");

  const auto modelsFolder = artifact.paths["modelDir"].toString();
  const auto sourceFiles = artifact.metadata["sources"].toStringList();
  const auto includeFolders = artifact.metadata["includes"].toStringList();

  for (const QString& file : sourceFiles)
  {
    if (!file.contains("_task"))
      continue;

    LOG_INFO("Will simulate: %s", qPrintable(file));

    mSimulator->setWorkingDirectory(modelsFolder);
    mSimulator->setSimulationModel(file);
    mSimulator->setSimulationIncludes(includeFolders);
    return mSimulator->startSimulation(QUuid::createUuid().toString());
  }

  return VoidResult();
}

QList<std::shared_ptr<maki::IPipelineAction>> KodaGenerator::pipelineActions()
{
  return {
      std::make_shared<GenerateKodaAction>(this),
      std::make_shared<GenerateDezyneAction>(this),
      std::make_shared<GenerateCppAction>(this),
      std::make_shared<KodaVerifyAction>(this),
      std::make_shared<KodaSimulateAction>(this),
      std::make_shared<GenerateRosAction>(),
      std::make_shared<KodaRosCopySources>(),
      std::make_shared<KodaRosBuild>(this),
      std::make_shared<KodaRosLaunch>(this),
  };
}

Result<maki::PipelineArtifact> KodaGenerator::generateDezyne(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("generateDezyne, missing input sources");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  // Make sure the output is clean before the generation
  auto modelsOutputFolder = QDir(outputFolder.absolutePath() + "/models");
  if (modelsOutputFolder.exists())
    modelsOutputFolder.removeRecursively();

  modelsOutputFolder.mkpath(".");

#ifdef USE_ANTLR
  koda::Compiler compiler;
  for (const auto& file : inputFiles)
  {
    koda::CompilerOptions options;
    options.inputFile = file.toStdString();
    options.outputDir = modelsOutputFolder.absolutePath().toStdString();
    LOG_DEBUG("Generating from file: %s to %s", qPrintable(file), qPrintable(outputFolder.absolutePath()));
    auto parsed = compiler.parse(options);
    if (!parsed)
      return Result<maki::PipelineArtifact>::Failed(parsed.ErrorMessage());
    auto generated = compiler.generate();
    if (!generated)
      return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());
  }

  QStringList includeFolders = {};
  QString libDstPath = modelsOutputFolder.absolutePath() + "/lib";
  if (mAssetDir)
  {
    LOG_DEBUG("Using asset dir: %s", qPrintable(mAssetDir->absolutePath()));
    QString libSrcPath = mAssetDir->absoluteFilePath("lib");
    auto copied = copyDirectory(libSrcPath, libDstPath);
    if (!copied.IsSuccess())
      return Result<maki::PipelineArtifact>::Failed(copied.ErrorMessage());

    includeFolders << libDstPath;
  }

  // Finally, set the files to be verified
  QStringList filters;
  filters << "*.dzn";

  QStringList outputFiles = {};
  for (const auto& file : modelsOutputFolder.entryList(filters, QDir::Files))
    outputFiles << modelsOutputFolder.absoluteFilePath(file);

  if (mAssetDir)
  {
    auto libDir = QDir(libDstPath);
    for (const auto& file : libDir.entryList(filters, QDir::Files))
      outputFiles << libDir.absoluteFilePath(file);
  }

  maki::PipelineArtifact output;
  output.paths = {
      {"modelDir", modelsOutputFolder.absolutePath()},
  };
  output.metadata = {
      {"sources", outputFiles},
      {"includes", includeFolders},
  };

  mDezyneOutputFolder = modelsOutputFolder;

  return output;
#else
  for (const auto& file : inputFiles)
  {
    LOG_DEBUG("Generating from file: %s to %s", qPrintable(file), qPrintable(outputFolder.absolutePath()));
    const QString command = "java";
    const QStringList arguments = {
        "-jar",
        QDir::homePath() + "/rascal-0.40.9.jar",
        "Main.rsc",                   // Entrypoint for KODA
        file,                         // Input
        outputFolder.absolutePath(),  // Output
        "ros"                         // Generator type
    };

    QProcess* generate = new QProcess(this);
    generate->setWorkingDirectory("/home/felaze/Documents/PhD/Programs/DSL/koda");
    generate->setProgram(command);
    generate->setArguments(arguments);

    pipeline->add(generate, maki::OnFail::STOP);
  }

  return maki::PipelineArtifact();
#endif
}

Result<maki::PipelineArtifact> KodaGenerator::generateCpp(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("generateCpp, missing input sources");
  else if (!artifact.metadata.contains("includes"))
    return Result<maki::PipelineArtifact>::Failed("Missing input includes");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  const QStringList includeFolders = artifact.metadata["includes"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  auto cppOutputFolder = QDir(outputFolder.absolutePath() + "/cpp");
  if (cppOutputFolder.exists())
    cppOutputFolder.removeRecursively();

  cppOutputFolder.mkpath(".");

  QStringList outputSourceFiles = {};
  QStringList outputHeaderFiles = {};

  pipeline->startGroup("CppGeneration");
  for (const auto& f : inputFiles)
  {
    auto fullPath = cppOutputFolder.absoluteFilePath(f);
    LOG_INFO("Will generate file: %s", qPrintable(fullPath));
    const QString command = "dzn";
    QStringList arguments = {
        "code",
        "-l",
        "c++",
        "-o",
        cppOutputFolder.absolutePath(),
        fullPath,
    };

    if (fullPath.contains("_task"))
    {
      QString name = QFileInfo(fullPath).completeBaseName();
      arguments << "-s" << "c" + name.replace("_task", "");
    }

    for (const auto& inc : includeFolders)
      arguments << "-I" << inc;

    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    QFileInfo info(fullPath);
    QString baseName = info.baseName();
    outputHeaderFiles << cppOutputFolder.absolutePath() + "/" + baseName + ".hh";
    outputSourceFiles << cppOutputFolder.absolutePath() + "/" + baseName + ".cc";

    pipeline->add(generate, maki::OnFail::STOP);
  }
  pipeline->endGroup();

  // Add dzn lib includes as well
  LOG_DEBUG("Using asset dir: %s", qPrintable(mAssetDir->absolutePath()));
  maki::PipelineArtifact output;
  output.paths = {
      {"includeDir", mAssetDir->absoluteFilePath("dzn_files/include")},
      {"sourceDir", {mAssetDir->absoluteFilePath("dzn_files/src")}},
  };
  output.metadata = {
      {"sources", outputSourceFiles},
      {"includes", outputHeaderFiles},
  };

  return output;
}

VoidResult KodaGenerator::verify(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  LOG_INFO("Running verification");

  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("verify, missing input sources");
  else if (!artifact.metadata.contains("includes"))
    return Result<maki::PipelineArtifact>::Failed("verify, missing input includes");
  else if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no services provided");
  else if (pipeline == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no pipeline provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no document provided");
  else if (!setup())
    return VoidResult::Failed("Could not setup plugin");

  if (mServices->document()->getnodes().isEmpty())
    return VoidResult::Failed("Nothing to verify");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  const QStringList includeFolders = artifact.metadata["includes"].toStringList();

  auto taskOnly = getSetting("taskOnly");
  pipeline->startGroup("Verification");
  for (const QString& f : inputFiles)
  {
    if (f.contains("/a_") || f.contains("types"))
      continue;
    if (f.contains("arbiter") || f.contains("/lib/"))
      continue;
    if (taskOnly.getValue().isValid() && taskOnly.getValue().toBool() && !f.contains("_task"))
      continue;

    LOG_DEBUG("Will verify file: %s", qPrintable(f));
#ifdef USE_ANTLR
    const QString command = "ide";
#else
    const QString command = "dzn";
#endif
    QStringList arguments = {"verify", f};
    for (const auto& inc : includeFolders)
      arguments << "-I" << inc;

    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    pipeline->add(generate, maki::OnFail::STOP);
  }
  pipeline->endGroup();

  return VoidResult();
}

Result<maki::PipelineArtifact> KodaGenerator::generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder)
{
  mOutputFolder = outputFolder;
  if (!mOutputFolder.exists())
    mOutputFolder.mkdir(".");

  LOG_DEBUG("Generating Koda files with %d nodes", mServices->document()->getnodes().size());
  QString code = "";

#ifdef USE_ANTLR
  koda::MakiToKoda makiToKoda;
  auto generated = makiToKoda.generate(mServices->document()->getnodes());
  if (!generated)
    return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());
#else
  auto generated = Result<QString>("This is a temp placeholder");
#endif

  QString fileName = mOutputFolder.filePath("task.kd");
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return Result<maki::PipelineArtifact>::Failed("Failed to open device for writing: " + fileName.toStdString());

  QTextStream out(&file);
  out << generated.Value();
  file.close();

  maki::PipelineArtifact output;
  output.metadata = {
      {"sources", {fileName}},
  };

  return output;
}

Result<maki::PipelineArtifact> KodaGenerator::buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!artifact.paths.contains("rootDir"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing root folder");

  const QString projectDir = artifact.paths["rootDir"].toString();
  const QString packageName = artifact.metadata["packageName"].toString();

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
      "-v", "/home/felaze/ros2_ws:/home/felaze/ros2_ws",
      "-v", projectDir + ":/home/felaze/ros2_ws/src/koda_ros",
      "-w", "/home/felaze/ros2_ws",
      "ros2:v1.0.0",
      "bash", "-ic", "source /opt/ros/humble/setup.bash && colcon build --symlink-install --event-handlers console_direct+",
  };
  // clang-format on
  generate->setArguments(args);

  pipeline->add(generate, maki::OnFail::STOP);
  pipeline->endGroup();

  return maki::PipelineArtifact{};
}

Result<maki::PipelineArtifact> KodaGenerator::launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
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
    "--device", "/dev/dri/card1:/dev/dri/card1",
    "--device", "/dev/dri/renderD128:/dev/dri/renderD128",
    "-v", "/tmp/.X11-unix:/tmp/.X11-unix:rw",
    "-v", "/home/felaze/ros2_ws:/home/felaze/ros2_ws:rw",
    "-v", projectDir + ":/home/felaze/ros2_ws/src/koda_ros:rw",
    "-w", "/home/felaze/ros2_ws",
    "ros2:v1.0.0",
    "bash", "-ic", "source /opt/ros/humble/setup.bash && source install/setup.bash && ros2 launch koda_ros koda.launch.py",
  };
  // clang-format on
  generate->setArguments(args);

  pipeline->add(generate, maki::OnFail::EXECUTE, [](int& exitCode, QProcess::ExitStatus& status) {
    // On end, stop the docker
    LOG_DEBUG("Stopping the docker container");
    exitCode = QProcess::execute("docker", {"stop", "-t", "5", "maki_koda_runtime"});
    if (exitCode != 0)
    {
      LOG_DEBUG("Failed to stop cleanly, killing the docker container");
      exitCode = QProcess::execute("docker", {"kill", "maki_koda_runtime"});
    }
    if (exitCode == 0)
      status = QProcess::ExitStatus::NormalExit;
  });
  pipeline->endGroup();

  return maki::PipelineArtifact{};
}

bool KodaGenerator::startDaemon()
{
  if (mDaemon != nullptr)
    return true;

  LOG_DEBUG("Starting daemon");
  mDaemon = new QProcess(this);

  connect(mDaemon, &QProcess::started, this, []() {
    LOG_DEBUG("Process started");
  });

  connect(mDaemon, &QProcess::readyReadStandardOutput, this, [this]() {
    LOG_DEBUG(mDaemon->readAllStandardOutput().trimmed().toStdString());
  });

  connect(mDaemon, &QProcess::readyReadStandardError, this, [this]() {
    LOG_DEBUG(mDaemon->readAllStandardError().trimmed().toStdString());
  });

  connect(mDaemon, &QProcess::finished, this, [](int exitCode, QProcess::ExitStatus status) {
    LOG_DEBUG("Daemon finished with code %d and status %d", exitCode, (int)status);
  });

  // connect(mDaemon, &QProcess::errorOccurred, this, [](QProcess::ProcessError e) {
  //   LOG_WARNING("Daemon error: %d", (int)e);
  // });

  // Non-blocking start
  QStringList arguments = {"daemon"};
  auto debug = getSetting("debug");
  if (debug.getValue().isValid() && debug.getValue().toBool())
    arguments.push_back("--debug");

  mDaemon->setProgram("ide");
  mDaemon->setArguments(arguments);
  mDaemon->start();

  LOG_DEBUG("Waiting for daemon to start");
  return mDaemon->waitForStarted() && mDaemon->waitForReadyRead();
}

void KodaGenerator::simulationStarted()
{
  if (!mServices)
    return;

  if (auto pluginTab = mServices->ui())
  {
    LOG_INFO("Simulation started");
    pluginTab->openScene(languageName());
  }
}

maki::SettingField KodaGenerator::getSetting(const QString& key) const
{
  for (const auto& s : mSettings)
    if (s.getKey() == key)
      return s;

  return maki::SettingField();
}

void KodaGenerator::simulationUpdated(const QJsonObject& obj)
{
  LOG_DEBUG("Simulation updated");
  mLastUpdate = obj;
  mServices->ui()->updateScene(languageName());
}

VoidResult KodaGenerator::createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj)
{
  // Nothing to render
  if (obj.isEmpty())
    return VoidResult();

  // auto pretty = QJsonDocument(obj).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Received message: %s", qPrintable(pretty));

  auto theme = mServices->ui()->currentTheme();
  if (!mTraceBuilder)
    mTraceBuilder = std::make_unique<TraceSceneBuilder>(theme, TraceSceneBuilder::Style{});

  auto clickHandler = [this](const QString& instance, const QString& labelText, bool /* illegal */) {
    LOG_DEBUG("Sending data: %s %s", qPrintable(instance), qPrintable(labelText));
    mSimulator->triggerEvent(labelText);
  };

  QString err;
  if (!mTraceBuilder->buildScene(obj, scene, clickHandler, &err))
    return VoidResult::Failed("Failed to build scene: " + err.toStdString());

  return VoidResult();
}
