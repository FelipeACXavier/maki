#pragma once

#include <QDir>
#include <QObject>

#include "generator_plugin.h"

class DezyneGenerator : public GeneratorPlugin
{
public:
  QString generateCode(const QList<NodeSaveInfo>& nodes) override;
  generator::Language supportedLanguage() const override;
  QString languageName() const override;

private:
  QDir mOutputFolder;
  QString generateNode(const NodeSaveInfo& node);
  QString generateChildCode(const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
};
