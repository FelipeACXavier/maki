#pragma once

#include <QObject>

#include "generator_plugin.h"

class JSONGenerator : public QObject, public GeneratorPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID GeneratorPlugin_iid)
  Q_INTERFACES(GeneratorPlugin)

public:
  QString generateCode(const QList<INode*>& nodes) override;
  generator::Language supportedLanguage() const override;
  QString languageName() const override;
};
