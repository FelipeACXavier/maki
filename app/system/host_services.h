#pragma once

#include <QObject>

#include "ihost_services.h"

namespace maki
{
class IPipeline;
class ISettings;
class ILogging;
class ITab;
}  // namespace maki

class IDocument;

class HostServices : public QObject, public maki::IHostServices
{
  Q_OBJECT
public:
  HostServices(IDocument* document, maki::IPipeline* pipeline, maki::ISettings* settings, const QString& root, QObject* parent = nullptr);

  void setPluginTab(maki::ITab* tab);
  void setLogger(maki::ILogging* logger);

  IDocument* document() const override;
  maki::ISettings* settings() const override;
  maki::IPipeline* pipeline() const override;
  QString projectRoot() const override;
  maki::ITab* pluginTab() const override;
  maki::ILogging* logger() const override;

private:
  IDocument* mDocument;
  maki::IPipeline* mPipeline;
  maki::ISettings* mSettings;
  maki::ITab* mPluginTab;
  maki::ILogging* mLogger;

  QString mRoot;
};