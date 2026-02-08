#include "host_services.h"

#include "idocument.h"
#include "ipipeline.h"

HostServices::HostServices(IDocument* document, maki::IPipeline* pipeline, const QString& root, QObject* parent)
    : QObject(parent)
    , mDocument(document)
    , mPipeline(pipeline)
    , mRoot(root)
{
}

IDocument* HostServices::document() const
{
  return mDocument;
}

maki::IPipeline* HostServices::pipeline() const
{
  return mPipeline;
}

QString HostServices::projectRoot() const
{
  return mRoot;
}