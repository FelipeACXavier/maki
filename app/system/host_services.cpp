#include "host_services.h"

#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "iui.h"

HostServices::HostServices(std::shared_ptr<IDocument> document, maki::IPipeline* pipeline, maki::ISettings* settings, const QString& root,
                           QObject* parent)
    : QObject(parent)
    , mDocument(document)
    , mPipeline(pipeline)
    , mSettings(settings)
    , mPluginTab(nullptr)
    , mRoot(root)
{
}

void HostServices::setPluginTab(maki::IUI* tab)
{
  mPluginTab = tab;
}

void HostServices::setLogger(maki::ILogging* logger)
{
  mLogger = logger;
}

IDocument* HostServices::document() const
{
  return mDocument.get();
}

maki::ISettings* HostServices::settings() const
{
  return mSettings;
}

maki::IPipeline* HostServices::pipeline() const
{
  return mPipeline;
}

maki::IUI* HostServices::ui() const
{
  return mPluginTab;
}

QString HostServices::projectRoot() const
{
  return mRoot;
}

maki::ILogging* HostServices::logger() const
{
  return mLogger;
}

void HostServices::errorOnNode(const QString& nodeId, const QString& flowId, const QString& message)
{
  emit onErrorOnNode(nodeId, flowId, message);
}