#include "koda_generator.h"

#include <qdir.h>
#include <qprocess.h>

#include <QApplication>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProcess>
#include <QTextStream>
#include <QTimer>
#include <filesystem>

#include "actions/cpp_action.h"
#include "actions/dezyne_action.h"
#include "actions/koda_action.h"
#include "actions/ros_action.h"
#include "actions/ros_build.h"
#include "actions/ros_copy.h"
#include "actions/ros_launch.h"
#include "actions/simulate_action.h"
#include "actions/verify_action.h"
#include "ast/koda_compiler.h"
#include "dzn_client/dezyne_simulator.h"
#include "dzn_client/simulation_scene.h"
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
#include "svg_tool_button.h"
#include "types.h"
#include "typing/type_registry.h"

#define APPEND_OR_RETURN_ON_FAILURE(v, func) \
  do                                         \
  {                                          \
    auto ret = func;                         \
    if (!ret.IsSuccess())                    \
      return ret;                            \
                                             \
    v += ret.Value();                        \
  } while (0)

struct Candidate
{
  QString instance;
  QVector<QString> labels;
};

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

  if (!mTraceMap)
    mTraceMap = std::make_shared<koda::TraceabilityMap>();

  // Start the ide daemon on a specific port
  return startDaemon();
}

bool KodaGenerator::tearDown()
{
  if (auto* tab = mServices->ui())
    tab->deregisterPlugin(languageName());

  return stopDaemon();
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
    pluginTab->registerPlugin(languageName(), [this](QGraphicsScene* scene) { return createSimulationScene(scene, mLastUpdate); });
}

void KodaGenerator::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

QList<std::shared_ptr<maki::IPipelineAction>> KodaGenerator::pipelineActions()
{
  return {
      std::make_shared<GenerateKodaAction>(this), std::make_shared<GenerateDezyneAction>(this), std::make_shared<GenerateCppAction>(this),
      std::make_shared<KodaVerifyAction>(this),   std::make_shared<KodaSimulateAction>(this),   std::make_shared<GenerateRosAction>(),
      std::make_shared<KodaRosCopySources>(),     std::make_shared<KodaRosBuild>(this),         std::make_shared<KodaRosLaunch>(this),
  };
}

Result<maki::PipelineArtifact> KodaGenerator::generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder)
{
  mOutputFolder = outputFolder;
  if (!mOutputFolder.exists())
    mOutputFolder.mkdir(".");

  LOG_DEBUG("Generating Koda files with {} nodes", mServices->document()->getnodes().size());
  QString code = "";

  // Clean before generation
  if (mTraceMap)
    mTraceMap->clear();

  const auto* typeRegistry = mServices->document()->getTypesRegistry();
  koda::MakiToKoda makiToKoda(typeRegistry, mTraceMap);
  const auto missionParameters = mServices->document()->getparameters();
  auto generated = makiToKoda.generate(mServices->document()->getnodes(), missionParameters);
  auto errors = makiToKoda.getErrors();
  if (!errors.empty())
  {
    auto firstError = errors.front();
    if (mServices)
      mServices->errorOnNode(firstError.nodeId, firstError.flowId, QString::fromStdString(firstError.message));
  }
  if (!generated)
    return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());

  mAST = std::make_shared<koda::System>(makiToKoda.getAST());

  QString fileName = mOutputFolder.filePath("task.kd");
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

Result<maki::PipelineArtifact> KodaGenerator::generateDezyne(const maki::PipelineArtifact& artifact, const QDir& outputFolder,
                                                             const maki::ValueMap& parameters, maki::IPipeline* pipeline)
{
  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("generateDezyne, missing input sources");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  // Make sure the output is clean before the generation
  auto modelsOutputFolder = QDir(outputFolder.absolutePath() + "/models");
  auto libOutputFolder = QDir(outputFolder.absolutePath() + "/models/lib");
  if (modelsOutputFolder.exists())
    modelsOutputFolder.removeRecursively();

  modelsOutputFolder.mkpath(".");

  koda::Compiler compiler;
  for (const auto& file : inputFiles)
  {
    koda::CompilerOptions options;
    if (parameters.contains("Simulate"))
      options.simulation = parameters.at("Simulate").toBool();
    if (parameters.contains("Start wait"))

      options.startWait = parameters.at("Start wait").toDouble();
    if (parameters.contains("Verbose"))
      options.verbose = 1;
    if (mAST && !(parameters.contains("From file") && parameters.at("From file").toBool()))
    {
      options.ast = mAST;
      options.typeRegistry = std::make_shared<koda::types::TypeRegistry>(*mServices->document()->getTypesRegistry());
    }

    options.traceability = mTraceMap;
    options.inputFile = file.toStdString();
    options.outputDir = modelsOutputFolder.absolutePath().toStdString();
    LOG_DEBUG("Generating from file: {} to {}", file, outputFolder.absolutePath());
    RETURN_ON_FAILURE_AS(compiler.parse(options), maki::PipelineArtifact);
    RETURN_ON_FAILURE_AS(compiler.generate(), maki::PipelineArtifact);
  }

  QStringList includeFolders = {};
  includeFolders << modelsOutputFolder.absolutePath();
  includeFolders << libOutputFolder.absolutePath();

  // Finally, set the files to be verified
  // We don't need to verify the library files
  QStringList filters;
  filters << "*.dzn";

  QStringList outputFiles = {};
  for (const auto& file : modelsOutputFolder.entryList(filters, QDir::Files))
    outputFiles << modelsOutputFolder.absoluteFilePath(file);

  for (const auto& file : libOutputFolder.entryList(filters, QDir::Files))
    outputFiles << libOutputFolder.absoluteFilePath(file);

  maki::PipelineArtifact output;
  output.paths = {
      {"modelDir", modelsOutputFolder.absolutePath()},
  };
  output.metadata = {
      {"sources", outputFiles},
      {"includes", includeFolders},
  };

  mDezyneOutputFolder = modelsOutputFolder;

  if (mTraceMap)
    mTraceMap->print();

  return output;
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
    LOG_INFO("Will generate file: {}", fullPath);
    const QString command = "dzn";
    QStringList arguments = {
        "code", "-l", "c++", "-o", cppOutputFolder.absolutePath(), fullPath,
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
  LOG_DEBUG("Using asset dir: {}", mAssetDir->absolutePath());
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

  if (mServices->document()->getnodes().isEmpty())
    return VoidResult::Failed("Nothing to verify");

  setup();

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

    LOG_DEBUG("Will verify file: {}", f);
    const QString command = "ide";
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

VoidResult KodaGenerator::simulate(const maki::PipelineArtifact& artifact)
{
  LOG_INFO("Running simulation");

  if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no services provided");
  else if (mServices->ui() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no plugin tab provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no document provided");

  if (!artifact.paths.contains("modelDir"))
    return VoidResult::Failed("No model folder provided");
  if (!artifact.metadata.contains("sources"))
    return VoidResult::Failed("No sources provided");
  if (!artifact.metadata.contains("includes"))
    return VoidResult::Failed("No includes provided");

  setup();

  const auto modelsFolder = artifact.paths["modelDir"].toString();
  const auto sourceFiles = artifact.metadata["sources"].toStringList();
  const auto includeFolders = artifact.metadata["includes"].toStringList();

  for (const QString& file : sourceFiles)
  {
    if (!file.contains("_task"))
      continue;

    LOG_INFO("Will simulate: {}", file);

    mSimulator->setWorkingDirectory(modelsFolder);
    mSimulator->setSimulationModel(file);
    mSimulator->setSimulationIncludes(includeFolders);
    return mSimulator->startSimulation(QUuid::createUuid().toString());
  }

  return VoidResult();
}

Result<maki::PipelineArtifact> KodaGenerator::buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder,
                                                              maki::IPipeline* pipeline)
{
  if (!artifact.paths.contains("rootDir"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing root folder");

  const QString projectDir = artifact.paths["rootDir"].toString();

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
      "bash", "-ic", "source /opt/ros/humble/setup.bash && colcon build --symlink-install",
  };
  // clang-format on
  generate->setArguments(args);

  pipeline->add(generate, maki::OnFail::STOP);
  pipeline->endGroup();

  return maki::PipelineArtifact{};
}

Result<maki::PipelineArtifact> KodaGenerator::launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder,
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
    "--device", "/dev/dri/card1:/dev/dri/card1",
    "--device", "/dev/dri/renderD128:/dev/dri/renderD128",
    "-v", "/tmp/.X11-unix:/tmp/.X11-unix:rw",
    "-v", "/home/felaze/ros2_ws:/home/felaze/ros2_ws:rw",
    "-v", projectDir + ":/home/felaze/ros2_ws/src/koda_ros:rw",
    "-w", "/home/felaze/ros2_ws",
    "ros2:v1.0.0",
    "bash", "-ic", QString("source /opt/ros/humble/setup.bash && source install/setup.bash && ros2 launch %1 %2 | grep -v \"Sensor origin\" | grep -v rviz").arg(packageName, launchFile),
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
  if (mDaemon && mDaemon->state() != QProcess::ProcessState::NotRunning)
    return true;

  LOG_DEBUG("Setting up daemon");
  stopDaemon();

  mDaemon = new QProcess(this);
  connect(mDaemon, &QProcess::started, this, []() { LOG_DEBUG("Process started"); });
  connect(mDaemon, &QProcess::readyReadStandardOutput, this, [this]() { LOG_DEBUG("{}", mDaemon->readAllStandardOutput().trimmed().toStdString()); });
  connect(mDaemon, &QProcess::readyReadStandardError, this, [this]() { LOG_DEBUG("{}", mDaemon->readAllStandardError().trimmed().toStdString()); });
  connect(mDaemon, &QProcess::finished, this,
          [](int exitCode, QProcess::ExitStatus status) { LOG_DEBUG("Daemon finished with code {} and status {}", exitCode, (int)status); });

  // Non-blocking start
  QStringList arguments = {"daemon"};
  auto debug = getSetting("debug");
  if (debug.getValue().isValid() && debug.getValue().toBool())
    arguments.push_back("--debug");

  mDaemon->setProgram("ide");
  mDaemon->setArguments(arguments);
  mDaemon->start();

  return mDaemon->waitForStarted();
}

bool KodaGenerator::stopDaemon()
{
  if (!mDaemon)
    return true;

  bool terminate = true;
  if (mDaemon->state() != QProcess::NotRunning)
  {
    mDaemon->terminate();
    terminate = mDaemon->waitForFinished(2500);
    if (!terminate)
    {
      mDaemon->kill();
      terminate = mDaemon->waitForFinished();
    }
  }

  mDaemon = nullptr;
  return terminate;
}

void KodaGenerator::simulationStarted()
{
  if (!mServices)
    return;

  mSimulator->triggerEvent("api.trigger");

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

  auto pretty = QJsonDocument(obj).toJson(QJsonDocument::Indented);
  LOG_DEBUG("Received message: {}", pretty.toStdString());

  auto theme = mServices->ui()->currentTheme();
  if (!mTraceBuilder)
    mTraceBuilder = std::make_unique<TraceSceneBuilder>(theme, TraceSceneBuilder::Style{});

  auto clickHandler = [this](const QString& instance, const QString& labelText, bool illegal) {
    LOG_DEBUG("Sending data: {} {}", instance, labelText);
    mSimulator->triggerEvent(labelText);
  };

  QString err;
  if (!mTraceBuilder->buildScene(obj, scene, clickHandler, &err))
    return VoidResult::Failed("Failed to build scene: " + err.toStdString());

  if (!mServices)
    return VoidResult();

  if (!mTraceMap)
    return VoidResult();

  if (!obj.contains("lifelines") || !obj.value("lifelines").isArray() || obj.value("lifelines").toArray().isEmpty())
  {
    LOG_WARNING("1");
    return VoidResult();
  }

  QVector<Candidate> candidates;
  for (const auto& ll : obj.value("lifelines").toArray())
  {
    auto lifeline = ll.toObject();
    if (!lifeline.contains("header") || !lifeline["header"].isObject())
    {
      LOG_WARNING("2");
      continue;
    }

    auto header = lifeline["header"].toObject();
    if (!header.contains("instance") || !header["instance"].isString())
    {
      LOG_WARNING("3");
      continue;
    }

    if (!lifeline.contains("labels") || !lifeline["labels"].isArray())
      continue;

    QStringList labels;
    for (const auto& l : lifeline["labels"].toArray())
    {
      auto label = l.toObject();
      if (label.contains("illegal") && label["illegal"].isBool() && label["illegal"].toBool())
        continue;

      if (!label.contains("role") || !label["role"].isString())
        continue;

      if (!label.contains("text") || !label["text"].isString())
        continue;

      const auto text = label["text"].toString();
      if (text == "<defer>")
      {
        QTimer::singleShot(0, [this, text]() {
          LOG_DEBUG("Sending defer");
          if (mSimulator)
            mSimulator->triggerEvent(text);
        });

        return VoidResult();
      }

      const auto role = label["role"].toString();
      if (role != "requires")
        continue;

      labels.push_back(text);
    }

    if (!labels.empty())
      candidates.append(Candidate{
          .instance = header["instance"].toString(),
          .labels = labels,
      });
  }

  for (const auto& candidate : candidates)
  {
    LOG_DEBUG("Trying to find node of candidate: {}", candidate.instance);
    auto source = mTraceMap->sourceForEmitter(candidate.instance.toStdString());
    if (!source)
      continue;

    auto* container = new QWidget();
    auto* containerLayout = new QHBoxLayout(container);
    containerLayout->setContentsMargins(4, 4, 4, 4);
    containerLayout->setSpacing(4);

    bool isSync = false;
    bool highlight = false;
    bool isFirst = true;
    for (const auto& label : candidate.labels)
    {
      // Sync tasks can only send Completed
      if (isSync)
        continue;

      QString text = "";
      QString icon = "";
      auto palette = scene->palette();
      if (source->kind == koda::MakiElementKind::Sync)
      {
        palette.setColor(QPalette::Button, QColor("#1ba8d5"));
        text = "Completed";
        icon = ":/icons/accept.svg";
        isSync = true;
      }
      else if (label.contains("Result:Failure"))
      {
        text = "Reject";
        palette.setColor(QPalette::Button, QColor("#e96b72"));
        icon = ":/icons/reject.svg";
      }
      else if (label.contains(".failure"))
      {
        palette.setColor(QPalette::Button, QColor("#e96b72"));
        text = "Failure";
        icon = ":/icons/reject.svg";
        highlight = true;
      }
      else if (label.contains("Result:Success"))
      {
        palette.setColor(QPalette::Button, QColor("#2bb5a0"));
        text = "Accept";
        icon = ":/icons/accept.svg";
      }
      else if (label.contains(".success"))
      {
        palette.setColor(QPalette::Button, QColor("#2bb5a0"));
        text = "Success";
        icon = ":/icons/accept.svg";
        highlight = true;
      }
      else
      {
        continue;
      }

      auto* labelLayout = new QVBoxLayout();
      labelLayout->setContentsMargins(0, 0, 0, 0);
      labelLayout->setSpacing(4);

      auto* qlabel = new QLabel(text, container);

      auto* button = new SvgToolButton(icon, container);
      button->setFixedSize(30, 30);
      button->setAutoRaise(true);

      connect(button, &SvgToolButton::clicked, this, [this, instance = candidate.instance, label] {
        LOG_DEBUG("Sending data: {} {}", instance, label);
        if (mSimulator)
          mSimulator->triggerEvent(label);
      });

      labelLayout->addWidget(button, 1, Qt::AlignVCenter | Qt::AlignHCenter);
      labelLayout->addWidget(qlabel, 0);

      if (!isFirst)
      {
        auto* line = new QFrame(container);
        line->setFrameShape(QFrame::VLine);
        containerLayout->addWidget(line);
      }

      containerLayout->addLayout(labelLayout, 1);
      isFirst = false;
    }

    mServices->simulateOnNode(QString::fromStdString(source.value().id), QString::fromStdString(source.value().flowId),
                              maki::SimulationProperties{
                                  .widget = container,
                                  .highlight = highlight = true,
                              });
  }

  return VoidResult();
}
