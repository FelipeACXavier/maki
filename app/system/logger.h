#pragma once
#include <QMap>
#include <QObject>
#include <QString>

#include "ilogging.h"
#include "logging.h"

namespace maki
{
class ILogging;
}

class Logger : public QObject, public maki::ILogging
{
public:
  Logger(QObject* parent = nullptr);

  void registerPlugin(const QString& name, maki::LogFunction& callback) override;

private:
  QMap<QString, maki::LogFunction> mPlugins;
};