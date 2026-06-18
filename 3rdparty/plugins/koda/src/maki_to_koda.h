
#pragma once

#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QString>
#include <memory>
#include <any>
#include <QDir>
#include <QFile>

#include "ast/ast.h"
#include "idocument.h"
#include "result.h"
#include "ihost_services.h"

class QJsonArray;

namespace koda
{

enum class TransitionKind
{
  Sequential,
  Error,
  Abort,
  Do,
  Else,
  Signal,
  Unknown
};

struct NodeTransition
{
  const INode* node;
  const ITransition* transition;
};

class MakiToKoda
{
public:
  Result<QString> generate(const QVector<std::shared_ptr<INode>> nodes);
  MakiToKoda(maki::IHostServices* services, QDir outputFolder);

private:
  // This should be a registry
  std::vector<std::string> mVariables = {};

  // Counter-based ID generation
  uint32_t mIdCounter = 0;
  std::string generateUniqueId();
  std::string generateUniqueName(const std::string& name);
  // Assumes name is already unique?
  QHash<QString, QString> mUniqueNames;

  // Reverse lookup maps: uniqueName -> shared_ptr to source object
  QHash<QString, std::shared_ptr<INode>> mUniqueToINode;        // uniqueName -> INode
  QHash<QString, std::shared_ptr<IFlow>> mUniqueToIFlow;        // uniqueName -> IFlow
  QHash<QString, std::shared_ptr<IProperty>> mUniqueToIProperty;// uniqueName -> IProperty
  QHash<QString, std::shared_ptr<ITransition>> mUniqueToITransition; // uniqueName -> ITransition

  // Register helpers (call these immediately after creating a unique name)
  void registerUniqueNameForINode(const std::string& uniqueName, std::shared_ptr<INode> node);
  void registerUniqueNameForIFlow(const std::string& uniqueName, std::shared_ptr<IFlow> flow);
  void registerUniqueNameForIProperty(const std::string& uniqueName, std::shared_ptr<IProperty> prop);
  void registerUniqueNameForITransition(const std::string& uniqueName, std::shared_ptr<ITransition> trans);

  // Lookup helpers (return nullptr / empty shared_ptr if not found)
  std::shared_ptr<INode> uniqueNameToINode(const std::string& uniqueName) const;
  std::shared_ptr<IFlow> uniqueNameToIFlow(const std::string& uniqueName) const;
  std::shared_ptr<IProperty> uniqueNameToIProperty(const std::string& uniqueName) const;
  std::shared_ptr<ITransition> uniqueNameToITransition(const std::string& uniqueName) const;

  Result<koda::PComponent> buildTask(const INode& task);
  Result<koda::PComponent> buildCapability(const INode& capability);

  Result<koda::PFlow> buildFlowAst(std::shared_ptr<IFlow> flow);
  std::any buildSequenceFrom(const IFlow& flow, const INode* start, const INode* stop);

  std::any buildJoinFromFanOut(const IFlow& flow, const INode& splitNode, const QList<NodeTransition>& successors, const INode*& joinNode);

  std::any buildNodeExpr(const IFlow& flow, const INode& node);
  std::any buildAsyncExpr(const IFlow& flow, const INode& node);
  std::any buildSyncExpr(const IFlow& flow, const INode& node);
  std::any buildStrategyExpr(const IFlow& flow, const INode& node);
  std::any buildWithinExpr(const IFlow& flow, const INode& node);
  std::any buildEveryExpr(const IFlow& flow, const INode& node);
  std::any buildRepeatExpr(const IFlow& flow, const INode& node);
  std::any buildContinueExpr(const IFlow& flow, const INode& node);
  std::any buildSuccessExpr(const IFlow& flow, const INode& node);
  QList<koda::PStrategyHandler> buildHandlers(const IFlow& flow, const INode& node);

  Result<koda::PVarDef> buildVarDef(const IProperty& property);
  Result<std::vector<koda::PActionDef>> buildActionDefs(const INode& node, const QJsonArray& typeArray);
  Result<koda::PRosDef> buildRosDef(const IFlow& flow);
  koda::PExpr buildExpr(const QJsonObject& object);
  std::vector<koda::PExpr> buildArgumentExpr(const QJsonArray& options, int start);

  QList<NodeTransition> sequentialPredecessorsOf(const INode& node, const IFlow& flow) const;
  QSet<QString> sequentiallyReachableFrom(const INode& node, const IFlow& flow) const;

  int maxSequentialDistanceFromBranches(const IFlow& flow, const QList<NodeTransition>& branches, const INode& target) const;
  int sequentialDistanceBetween(const IFlow& flow, const INode& start, const INode& target) const;

  const INode* findStartNode(const IFlow& flow) const;
  const INode* findDestination(const QString& dstId, const IFlow& flow) const;

  QSet<QString> reachableFrom(const INode& node, const IFlow& flow) const;
  const INode* findNearestCommonJoin(const IFlow& flow, const QList<NodeTransition>& branches) const;

  bool isEndNode(const INode& node) const;
  bool isStructuralNode(const INode& node) const;

  int maxDistanceFromBranches(const IFlow& flow, const QList<const INode*>& branches, const INode& target) const;

  int distanceBetween(const IFlow& flow, const INode& start, const INode& target) const;

  QList<NodeTransition> successorsOfKind(const INode& node, const IFlow& flow, TransitionKind kind) const;
  QList<NodeTransition> sequentialSuccessorsOf(const INode& node, const IFlow& flow) const;
  QList<NodeTransition> errorSuccessorsOf(const INode& node, const IFlow& flow) const;
  QList<NodeTransition> abortSuccessorsOf(const INode& node, const IFlow& flow) const;
  QList<NodeTransition> doSuccessorsOf(const INode& node, const IFlow& flow) const;
  QList<NodeTransition> elseSuccessorsOf(const INode& node, const IFlow& flow) const;
  QList<NodeTransition> signalSuccessorsOf(const INode& node, const IFlow& flow) const;

  bool isSequentialTransition(const ITransition& transition) const;
  bool isAbortTransition(const ITransition& transition) const;
  bool isErrorTransition(const ITransition& transition) const;
  bool isDoTransition(const ITransition& transition) const;
  bool isElseTransition(const ITransition& transition) const;
  bool isSignalTransition(const ITransition& transition) const;

  TransitionKind transitionKind(const ITransition& transition) const;

  std::string format(QString input, const QString& token = "") const;

  maki::IHostServices* mServices = nullptr;
  QDir mOutputFolder;
  bool writeJsonToFile(const QString& path, const QJsonObject& obj);
};

}  // namespace koda
