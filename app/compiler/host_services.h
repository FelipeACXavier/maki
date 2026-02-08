#pragma once

#include <QObject>

#include "ihost_services.h"

namespace maki
{
class IPipeline;
}

class IDocument;

class HostServices : public QObject, public maki::IHostServices
{
  Q_OBJECT
public:
  HostServices(IDocument* document, maki::IPipeline* pipeline, const QString& root, QObject* parent = nullptr);

  IDocument* document() const override;
  maki::IPipeline* pipeline() const override;
  QString projectRoot() const override;

private:
  IDocument* mDocument;
  maki::IPipeline* mPipeline;

  QString mRoot;
};