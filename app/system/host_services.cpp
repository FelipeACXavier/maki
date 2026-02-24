#include "host_services.h"

#include "idocument.h"
#include "ipipeline.h"

HostServices::HostServices(IDocument* document, maki::IPipeline* pipeline, maki::ISettings* settings, const QString& root, QObject* parent)
    : QObject(parent)
    , mDocument(document)
    , mPipeline(pipeline)
    , mSettings(settings)
    , mPluginTab(nullptr)
    , mRoot(root)
{
}

void HostServices::setPluginTab(maki::ITab* tab)
{
  mPluginTab = tab;
}

IDocument* HostServices::document() const
{
  return mDocument;
}

maki::ISettings* HostServices::settings() const
{
  return mSettings;
}

maki::IPipeline* HostServices::pipeline() const
{
  return mPipeline;
}

maki::ITab* HostServices::pluginTab() const
{
  return mPluginTab;
}

QString HostServices::projectRoot() const
{
  return mRoot;
}