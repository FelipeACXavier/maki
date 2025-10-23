#pragma once

#include <QDir>
#include <QObject>

#include "compiler/generator_plugin.h"

class RozyneGenerator : public QObject, public GeneratorPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID GeneratorPlugin_iid FILE "rozyne_generator.json")
  Q_INTERFACES(GeneratorPlugin)

public:
  QString generateCode(std::shared_ptr<SaveInfo> nodes) override;
  generator::Language supportedLanguage() const override;
  QString languageName() const override;

private:
  QDir mOutputFolder;
  std::shared_ptr<SaveInfo> mStorage;
  QVector<QString> mImports;

  struct Argument
  {
    QString name = "";
  };

  // Generic generators
  QString generateNode(const NodeSaveInfo& node);
  QString generateBehaviourNode(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateTransitions(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);

  // These are the block generators
  QString generateComponent(const NodeSaveInfo& node, const QString& code, const QString& args);
  QString generateCapability(const NodeSaveInfo& node);

  QString generateStart(const QString& parent, const NodeSaveInfo& node, const FlowSaveInfo& flow, const QString& format);
  QString generateEnd(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateError(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);

  QString generateAsyncTask(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateSyncTask(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateWithin(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateRepeat(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateStrategy(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);

  // Helpers
  QString fixCase(const QString& name);
  std::shared_ptr<NodeSaveInfo> findDestination(const QString& nodeId, const FlowSaveInfo& flow) const;
  QVector<QString> mGeneratedIds;
};
