#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>

#include "generator_plugin.h"
#include "isettings.h"

class ProjectPage;
class QPlainTextEdit;

namespace maki
{

class ProjectPage;

class PlatformIOPlugin : public QObject, public maki::IGeneratorPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "platformio.json")
  Q_INTERFACES(maki::IGeneratorPlugin)

public:
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;
  void setName(const QString& name) override;
  void setVersion(const QString& name) override;
  void setAssetDir(const QDir& dir) override;

  QString languageName() const override;
  maki::PluginVersion version() const override;

  VoidResult verify(const QString& outputFolder) override;
  VoidResult simulate(const QString& outputFolder) override;

  QList<QString> generatedFiles() const override;
  void settingsChanged(const QVector<maki::SettingField>& settings) override;

private:
  maki::PluginVersion mVersion;
  QString mName;
  std::optional<QDir> mAssetDir;

  maki::IHostServices* mServices = nullptr;

  QString mPioExecutable;
  QString mCurrentOutputFolder;

  maki::ProjectPage* mProjectPage = nullptr;
  QPlainTextEdit* mOutputPanel = nullptr;

  QProcess* mMonitorProcess = nullptr;

  QVector<maki::SettingField> mSettings = {};
  QList<QString> mGeneratedFiles = {};

  VoidResult initialiseProject(const QString& outputFolder);
  VoidResult writePlatformIni(const QString& outputFolder);
  VoidResult writeMainCpp(const QString& outputFolder);

  void refreshDevices();
  void appendOutput(const QString& text);

  VoidResult runPio(const QString& workingDir, const QStringList& arguments, bool waitForFinished = true);

  void buildSettings();
};

}  // namespace maki