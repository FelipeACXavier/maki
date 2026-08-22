
#pragma once

#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <any>

#include "ast/ast.h"
#include "idocument.h"
#include "mission_parameter.h"
#include "result.h"
#include "typing/type_registry.h"

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
  MakiToKoda(const koda::types::TypeRegistry* registry);

  Result<QString> generate(const QVector<std::shared_ptr<INode>> nodes, QVector<const IParameter*> parameters);

private:
  const koda::types::TypeRegistry* mTypeRegistry;

  Result<koda::PComponent> buildTask(const INode& task, QVector<const IParameter*> missionParameters);
  Result<koda::PComponent> buildCapability(const INode& capability);

  Result<koda::PFlow> buildFlowAst(const IFlow& flow);
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

  Result<koda::PVarDef> buildVarDef(const IParameter* property);
  Result<std::vector<koda::PActionDef>> buildActionDefs(const INode& node, const maki::ListValue& definition);
  Result<koda::PRosDef> buildRosDef(const IFlow& flow);
  koda::PExpr buildExpr(const maki::Value& object);
  std::vector<koda::PExpr> buildArgumentExpr(const maki::ListValue& options);

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

  Result<koda::PExpr> buildValueExpr(const koda::types::TypeReference& type, const maki::Value* value);
  const maki::Value* getProperty(const QString& key, const INode& node) const;
};

}  // namespace koda
