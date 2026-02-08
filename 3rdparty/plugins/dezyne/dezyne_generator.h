#pragma once

#include <QDir>
#include <QObject>

#include "elements/save_info.h"
#include "compiler/generator_plugin.h"

class DezyneGenerator : public GeneratorPlugin
{
  Q_PLUGIN_METADATA(IID GeneratorPlugin_iid FILE "dezynegenerator.json")
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
  QString generateTimer(const NodeSaveInfo& node);
  QString generateAuthenticator(const NodeSaveInfo& node);
  QString generateSiren(const NodeSaveInfo& node);
  QString generatePresenceSensor(const NodeSaveInfo& node);
  QString generateComponent(const NodeSaveInfo& node);
  QString generateInterface(const NodeSaveInfo& node);
  QString generateBehaviour(const FlowSaveInfo& node);

  QString generateStart(const QString& parent, const NodeSaveInfo& node, const FlowSaveInfo& flow, const QString& format);
  QString generateEnd(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateError(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateAction(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateCondition(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateAssign(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);
  QString generateState(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format);

  // Helpers
  QString fixCase(const QString& name);
  std::shared_ptr<NodeSaveInfo> findDestination(const QString& nodeId, const FlowSaveInfo& flow) const;
  QVector<QString> mGeneratedIds;
};
