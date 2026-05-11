#pragma once

#include <QDir>
#include <QGraphicsItem>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

#include "ihost_services.h"
#include "isettings.h"
#include "pipeline_action.h"
#include "plugin_version.h"
#include "result.h"

namespace maki
{

class IPlugin
{
public:
  virtual ~IPlugin() = default;

  virtual bool setup() = 0;
  virtual bool tearDown() = 0;
  virtual void setHostServices(maki::IHostServices* services) = 0;
  virtual void setName(const QString& name) = 0;
  virtual void setVersion(const QString& name) = 0;
  virtual void setAssetDir(const QDir& dir) = 0;

  virtual QString languageName() const = 0;
  virtual PluginVersion version() const = 0;

  virtual QList<QString> generatedFiles() const = 0;
  virtual void settingsChanged(const QVector<maki::SettingField>& settings) = 0;

  virtual QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() = 0;
};
}  // namespace maki

Q_DECLARE_METATYPE(maki::PluginVersion);

#define MAKI_GENERATORPLUGIN_IID "com.maki.GeneratorPlugin/1.0"
Q_DECLARE_INTERFACE(maki::IPlugin, MAKI_GENERATORPLUGIN_IID)
