#include "platformio.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QPlainTextEdit>
#include <QStringList>

#include "actions/build_project.h"
#include "actions/copy_sources.h"
#include "actions/create_project.h"
#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "iui.h"
#include "logging.h"
#include "project_page.h"

namespace maki
{

bool PlatformIOPlugin::setup()
{
  // Use the full path for the command
  const QString home = QDir::homePath();
#ifdef Q_OS_WIN
  mPioExecutable = QDir(home).filePath(".platformio/penv/Scripts/pio.exe");
#else
  mPioExecutable = QDir(home).filePath(".platformio/penv/bin/pio");
#endif

  if (!mProjectPage)
    mProjectPage = new ProjectPage();

  // connect(mProjectPage, &maki::ProjectPage::refreshDevicesRequested, this, &PlatformIOPlugin::refreshDevices);

  // connect(mProjectPage, &maki::ProjectPage::generateRequested, this, [this] {
  //   generate(mCurrentOutputFolder);
  // });

  // connect(mProjectPage, &maki::ProjectPage::buildRequested, this, [this] {
  //   build(mCurrentOutputFolder);
  // });

  // connect(mProjectPage, &maki::ProjectPage::uploadRequested, this, [this] {
  //   upload(mCurrentOutputFolder);
  // });

  // connect(mProjectPage, &maki::ProjectPage::monitorRequested, this, [this] {
  //   monitor(mCurrentOutputFolder);
  // });

  // if (mServices)
  // {
  //   mServices->addMainTab(
  //       "platformio.project",
  //       "PlatformIO",
  //       mProjectPage,
  //       QIcon(":/icons/board.svg"));

  //   mOutputPanel = new QPlainTextEdit();
  //   mOutputPanel->setReadOnly(true);

  //   mServices->addBottomPanel(
  //       "platformio.output",
  //       "PlatformIO Output",
  //       mOutputPanel,
  //       QIcon(":/icons/terminal.svg"));

  //   auto* buildAction = new QAction(QIcon(":/icons/build.svg"), "Build firmware", this);
  //   auto* uploadAction = new QAction(QIcon(":/icons/upload.svg"), "Upload firmware", this);
  //   auto* monitorAction = new QAction(QIcon(":/icons/terminal.svg"), "Serial monitor", this);

  //   connect(buildAction, &QAction::triggered, this, [this] {
  //     build(mCurrentOutputFolder);
  //   });

  //   connect(uploadAction, &QAction::triggered, this, [this] {
  //     upload(mCurrentOutputFolder);
  //   });

  //   connect(monitorAction, &QAction::triggered, this, [this] {
  //     monitor(mCurrentOutputFolder);
  //   });

  //   mServices->addToolBarAction("platformio", buildAction);
  //   mServices->addToolBarAction("platformio", uploadAction);
  //   mServices->addToolBarAction("platformio", monitorAction);
  // }

  return true;
}

bool PlatformIOPlugin::tearDown()
{
  if (mMonitorProcess)
  {
    mMonitorProcess->terminate();
    mMonitorProcess->waitForFinished(1000);
    mMonitorProcess->deleteLater();
    mMonitorProcess = nullptr;
  }

  return true;
}

void PlatformIOPlugin::buildSettings()
{
  maki::SettingField enableAllProjectConfigs;
  enableAllProjectConfigs.setKey("enableAllProjectConfigs");
  enableAllProjectConfigs.setLabel("Enable all project configs");
  enableAllProjectConfigs.setDescription("Only recommended if you know what you are doing");
  enableAllProjectConfigs.setDefaultValue(false);
  enableAllProjectConfigs.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(enableAllProjectConfigs);
}

void PlatformIOPlugin::setHostServices(maki::IHostServices* services)
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
}

void PlatformIOPlugin::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

QList<std::shared_ptr<maki::IPipelineAction>> PlatformIOPlugin::pipelineActions()
{
  return {
      std::make_shared<PlatformIOCreateProject>(this),
      std::make_shared<PlatformIOBuildAction>(this),
      std::make_shared<PlatformIOCopySources>(this),
  };
}

Result<maki::PipelineArtifact> PlatformIOPlugin::initialiseProject(const QDir& outputFolder, maki::IPipeline* pipeline)
{
  LOG_DEBUG("Initialising PlatformIO project");

  if (!setup())
    return Result<maki::PipelineArtifact>::Failed("Failed to setup plugin");
  else if (!pipeline)
    return Result<maki::PipelineArtifact>::Failed("No pipeline provided");

  auto projectFolder = outputFolder;
  if (projectFolder.exists())
    projectFolder.removeRecursively();

  if (!projectFolder.mkpath("."))
    return Result<maki::PipelineArtifact>::Failed("Could not create PlatformIO output folder");

  pipeline->startGroup("Project creation");
  const auto profile = mProjectPage->profile();

  QStringList args = {"project", "init", "--project-dir", projectFolder.absolutePath(), "--board", profile.board};

  QProcess* generate = new QProcess(this);
  generate->setWorkingDirectory(projectFolder.absolutePath());
  generate->setProgram(mPioExecutable);
  generate->setArguments(args);
  auto added = pipeline->add(generate, maki::OnFail::ALWAYS_EXECUTE, [this, projectFolder](int& /* exitCode */, QProcess::ExitStatus& /* status */) {
    LOG_WARN_ON_FAILURE(writePlatformIni(projectFolder));
  });

  pipeline->endGroup();

  if (!added.IsSuccess())
    return Result<maki::PipelineArtifact>::Failed(added.ErrorMessage());

  maki::PipelineArtifact artifact;
  artifact.paths = {
      {"includeDir", projectFolder.absolutePath() + "/include"},
      {"sourceDir", projectFolder.absolutePath() + "/src"},
      {"libraryDir", projectFolder.absolutePath() + "/lib"},
      {"rootDir", projectFolder.absolutePath()},
  };
  artifact.metadata = {
      {"ini", projectFolder.absolutePath() + "/platform.ini"},
  };

  return artifact;
}

Result<maki::PipelineArtifact> PlatformIOPlugin::buildProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!setup())
    return Result<maki::PipelineArtifact>::Failed("Failed to setup plugin");
  else if (!artifact.paths.contains("rootDir"))
    return Result<maki::PipelineArtifact>::Failed("buildProject, missing root folder");

  const QString projectDir = artifact.paths["rootDir"].toString();
  const auto profile = mProjectPage->profile();

  pipeline->startGroup("Generation");

  QProcess* generate = new QProcess(this);
  generate->setWorkingDirectory(projectDir);
  generate->setProgram(mPioExecutable);
  QStringList args = {"run", "-d", projectDir, "-e", profile.environment};
  generate->setArguments(args);
  pipeline->add(generate, maki::OnFail::STOP);

  pipeline->endGroup();

  maki::PipelineArtifact firmware;
  firmware.paths = {
      {"buildDir", projectDir + "/build"},
  };
  firmware.metadata = {
      {"firmware", projectDir + "/build/myexecutable"},
  };

  return firmware;
}

VoidResult PlatformIOPlugin::writePlatformIni(const QDir& outputFolder)
{
  const auto profile = mProjectPage->profile();

  LOG_DEBUG("Writing platform ini file on: %s", qPrintable(outputFolder.absolutePath()));

  QFile file(outputFolder.filePath("platformio.ini"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return VoidResult::Failed("Could not write platformio.ini");

  QTextStream out(&file);

  out << "[env:" << profile.environment << "]\n";
  out << "platform = " << profile.platform << "\n";
  out << "board = " << profile.board << "\n";
  out << "framework = " << profile.framework << "\n";
  out << "monitor_speed = " << profile.monitorSpeed << "\n";
  out << "build_unflags = -fno-rtti -std=gnu++11\n";
  out << "build_flags = -g -std=c++17 -std=gnu++17 -DCONFIG_ARDUINO_LOOP_STACK_SIZE=16384\n";
  out << "lib_ldf_mode = deep+\n";

  if (!profile.uploadPort.isEmpty())
    out << "upload_port = " << profile.uploadPort << "\n";

  if (!profile.monitorPort.isEmpty())
    out << "monitor_port = " << profile.monitorPort << "\n";

  return VoidResult();
}

VoidResult PlatformIOPlugin::writeMainCpp(const QString& outputFolder)
{
  QDir dir(outputFolder);
  if (!dir.mkpath("src"))
    return VoidResult::Failed("Could not create src folder");

  QFile file(dir.filePath("src/main.cpp"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return VoidResult::Failed("Could not write src/main.cpp");

  QTextStream out(&file);

  out << R"(#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
)";

  return VoidResult();
}

VoidResult PlatformIOPlugin::runPio(const QString& workingDir, const QStringList& arguments, bool waitForFinished)
{
  auto* process = new QProcess(this);
  process->setProgram(mPioExecutable);
  process->setArguments(arguments);
  process->setWorkingDirectory(workingDir);
  process->setProcessChannelMode(QProcess::MergedChannels);

  connect(process, &QProcess::readyReadStandardOutput, this, [this, process] {
    appendOutput(QString::fromUtf8(process->readAllStandardOutput()));
  });

  connect(process, &QProcess::readyReadStandardError, this, [this, process] {
    appendOutput(QString::fromUtf8(process->readAllStandardError()));
  });

  connect(process, &QProcess::finished, this, [this, process](int exitCode, QProcess::ExitStatus status) {
    appendOutput(QString("PlatformIO finished with exit code %1").arg(exitCode));

    if (process == mMonitorProcess)
      mMonitorProcess = nullptr;

    process->deleteLater();
  });

  process->start();

  if (!process->waitForStarted())
  {
    process->deleteLater();
    return VoidResult::Failed("Could not start PlatformIO. Is `pio` installed and available on PATH?");
  }

  if (!waitForFinished)
  {
    mMonitorProcess = process;
    return VoidResult();
  }

  process->waitForFinished(-1);

  if (process->exitStatus() != QProcess::NormalExit || process->exitCode() != 0)
    return VoidResult::Failed("PlatformIO command failed");

  return VoidResult();
}

void PlatformIOPlugin::refreshDevices()
{
  QProcess process;
  process.setProgram(mPioExecutable);
  process.setArguments({"device", "list", "--json-output"});
  process.setProcessChannelMode(QProcess::MergedChannels);

  process.start();

  if (!process.waitForStarted())
  {
    appendOutput("Could not start PlatformIO to refresh devices");
    return;
  }

  process.waitForFinished();

  const QByteArray raw = process.readAllStandardOutput();
  const QJsonDocument doc = QJsonDocument::fromJson(raw);

  QStringList devices;
  if (doc.isArray())
  {
    for (const auto& value : doc.array())
    {
      const QJsonObject object = value.toObject();

      const QString port = object.value("port").toString();
      const QString description = object.value("description").toString();

      if (port.isEmpty())
        continue;

      if (description.isEmpty())
        devices << port;
      else
        devices << QString("%1").arg(port);
    }
  }

  if (mProjectPage)
    mProjectPage->setDevices(devices);

  appendOutput(QString("Detected %1 PlatformIO device(s)").arg(devices.size()));
}

void PlatformIOPlugin::appendOutput(const QString& text)
{
  if (text.trimmed().isEmpty())
    return;

  if (mProjectPage)
    mProjectPage->appendOutput(text);

  if (mOutputPanel)
    mOutputPanel->appendPlainText(text.trimmed());

  if (mServices)
    LOG_INFO(qPrintable(text.trimmed()));
}

void PlatformIOPlugin::settingsChanged(const QVector<maki::SettingField>& /* settings */)
{
}

}  // namespace maki