#pragma once

#include <QGraphicsItem>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

#include "ihost_services.h"

namespace maki
{
struct PluginVersion
{
  QString major = "";
  QString minor = "";
  QString patch = "";

  QString toString() const
  {
    return QString("%1.%2.%3").arg(major, minor, patch);
  }

  static PluginVersion fromString(const QString& s)
  {
    PluginVersion version;
    const QStringList parts = s.split('.', Qt::KeepEmptyParts);

    if (parts.size() != 3)
      return version;

    version.major = parts.at(0).trimmed();
    version.minor = parts.at(1).trimmed();
    version.patch = parts.at(2).trimmed();

    return version;
  }

  bool operator==(const PluginVersion& other) const
  {
    return major == other.major &&
           minor == other.minor &&
           patch == other.patch;
  }

  bool operator!=(const PluginVersion& other) const
  {
    return !(*this == other);
  }
};

class IGeneratorPlugin
{
public:
  virtual ~IGeneratorPlugin() = default;

  virtual bool setup() = 0;
  virtual bool tearDown() = 0;
  virtual void setHostServices(maki::IHostServices* services) = 0;

  virtual QString languageName() const = 0;
  virtual PluginVersion version() const = 0;

  virtual QString verify(const QString& outputFolder) = 0;
  virtual QString simulate(const QString& outputFolder) = 0;

  virtual QList<QString> generatedFiles() const = 0;
};
}  // namespace maki

Q_DECLARE_METATYPE(maki::PluginVersion);

#define MAKI_GENERATORPLUGIN_IID "com.maki.GeneratorPlugin/1.0"
Q_DECLARE_INTERFACE(maki::IGeneratorPlugin, MAKI_GENERATORPLUGIN_IID)