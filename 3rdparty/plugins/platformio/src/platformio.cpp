#include "platformio.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QPlainTextEdit>
#include <QStringList>

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
    logging::gSilentLog = true;
    logger->registerPlugin(languageName(), logging::gLogToStream);
  }

  buildSettings();

  // Setup settings
  if (auto service = mServices->settings())
  {
    service->registerSettings(languageName(), version(), mSettings);

    auto settings = service->getPluginSettings(languageName());
    if (!settings.isEmpty())
      mSettings = settings;
  }

  if (auto ui = mServices->ui())
  {
  }
}

void PlatformIOPlugin::setName(const QString& name)
{
  mName = name;
}

void PlatformIOPlugin::setVersion(const QString& version)
{
  mVersion = PluginVersion::fromString(version);
}

void PlatformIOPlugin::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

QString PlatformIOPlugin::languageName() const
{
  return mName;
}

PluginVersion PlatformIOPlugin::version() const
{
  return mVersion;
}

// VoidResult PlatformIOPlugin::generate(const QString& outputFolder)
VoidResult PlatformIOPlugin::verify(const QString& outputFolder)
{
  if (outputFolder.isEmpty())
    return VoidResult::Failed("No output folder configured for PlatformIO generation");
  else if (mServices->pipeline() == nullptr)
    return VoidResult::Failed("Cannot proceed with generation, no pipeline provided");

  mGeneratedFiles = {};
  mCurrentOutputFolder = outputFolder;

  if (auto r = initialiseProject(outputFolder); !r)
    return r;

  // if (auto r = writePlatformIni(outputFolder); !r)
  //   return r;

  // if (auto r = writeMainCpp(outputFolder); !r)
  //   return r;

  // appendOutput("Generated PlatformIO project in: " + outputFolder);

  return VoidResult();
}

VoidResult PlatformIOPlugin::simulate(const QString& outputFolder)
{
  return verify(outputFolder);
}

QList<QString> PlatformIOPlugin::generatedFiles() const
{
  return mGeneratedFiles;
}

// VoidResult PlatformIOPlugin::build(const QString& outputFolder)
// {
//   if (auto r = generate(outputFolder); !r)
//     return r;

//   const auto profile = mProjectPage->profile();

//   QStringList args = {"run", "-e", profile.environment};

//   return runPio(outputFolder, args);
// }

// VoidResult PlatformIOPlugin::upload(const QString& outputFolder)
// {
//   if (auto r = generate(outputFolder); !r)
//     return r;

//   const auto profile = mProjectPage->profile();

//   QStringList args = {"run", "-e", profile.environment, "-t", "upload"};

//   if (!profile.uploadPort.isEmpty())
//     args << "--upload-port" << profile.uploadPort;

//   return runPio(outputFolder, args);
// }

// VoidResult PlatformIOPlugin::monitor(const QString& outputFolder)
// {
//   if (outputFolder.isEmpty())
//     return VoidResult::Failed("No output folder configured for PlatformIO monitor");

//   const auto profile = mProjectPage->profile();

//   QStringList args = {
//       "device",
//       "monitor",
//       "-d",
//       outputFolder,
//       "-e",
//       profile.environment,
//       "--baud",
//       QString::number(profile.monitorSpeed)};

//   if (!profile.monitorPort.isEmpty())
//     args << "--port" << profile.monitorPort;

//   return runPio(outputFolder, args, false);
// }

VoidResult PlatformIOPlugin::initialiseProject(const QString& outputFolder)
{
  LOG_DEBUG("Initialising PlatformIO project");

  QDir dir(outputFolder);
  if (!dir.exists() && !dir.mkpath("."))
    return VoidResult::Failed("Could not create PlatformIO output folder");

  const auto profile = mProjectPage->profile();

  mServices->pipeline()->startGroup("Generation");

  QStringList args = {"project", "init", "--project-dir", outputFolder, "--board", profile.board};

  QProcess* generate = new QProcess(this);
  generate->setWorkingDirectory(outputFolder);
  generate->setProgram(mPioExecutable);
  generate->setArguments(args);
  mServices->pipeline()->add(generate, maki::OnFail::EXECUTE, [this, outputFolder] {
    LOG_WARN_ON_FAILURE(writePlatformIni(outputFolder));
    LOG_WARN_ON_FAILURE(writeMainCpp(outputFolder));
  });

  mServices->pipeline()->endGroup();

  return VoidResult();
}

VoidResult PlatformIOPlugin::writePlatformIni(const QString& outputFolder)
{
  const auto profile = mProjectPage->profile();

  LOG_DEBUG("Writing platform ini file on: %s", qPrintable(outputFolder));

  QFile file(QDir(outputFolder).filePath("platformio.ini"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return VoidResult::Failed("Could not write platformio.ini");

  QTextStream out(&file);

  out << "[env:" << profile.environment << "]\n";
  out << "platform = " << profile.platform << "\n";
  out << "board = " << profile.board << "\n";
  out << "framework = " << profile.framework << "\n";
  out << "monitor_speed = " << profile.monitorSpeed << "\n";

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