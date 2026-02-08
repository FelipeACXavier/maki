#pragma once

#include <QGraphicsItem>
#include <QList>
#include <QObject>
#include <QString>

#include "ihost_services.h"

namespace maki
{
class IGeneratorPlugin
{
public:
  virtual ~IGeneratorPlugin() = default;

  virtual bool setup() = 0;
  virtual bool tearDown() = 0;
  virtual void setHostServices(maki::IHostServices* services) = 0;

  virtual QString languageName() const = 0;

  virtual QString verify(const QString& outputFolder) = 0;
  virtual QString simulate(const QString& outputFolder) = 0;

  virtual QList<QString> generatedFiles() const = 0;
};
}  // namespace maki

#define MAKI_GENERATORPLUGIN_IID "com.maki.GeneratorPlugin/1.0"

Q_DECLARE_INTERFACE(maki::IGeneratorPlugin, MAKI_GENERATORPLUGIN_IID)
