
#pragma once

#include <QHash>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>

#include "idocument.h"
#include "result.h"

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

struct KodaExpr
{
  enum class Kind
  {
    Empty,
    Call,
    Sequence,
    Join,
    Handler,
    Branch,
    End
  };
  Kind kind = Kind::Empty;
  QString text;
  QList<KodaExpr> children;
};

struct NodeTransition
{
  const INode* node;
  const ITransition* transition;
};

class MakiToKoda
{
public:
  Result<QString> generateFlow(const INode& owner, const IFlow& flow, const QString& indent = " ");

private:
  Result<KodaExpr> buildFlowAst(const IFlow& flow);
  Result<KodaExpr> buildSequenceFrom(const IFlow& flow, const INode* start, const INode* stop);

  Result<KodaExpr> buildJoinFromFanOut(const IFlow& flow, const INode& splitNode, const QList<NodeTransition>& successors, const INode*& joinNode);

  Result<KodaExpr> buildNodeExpr(const IFlow& flow, const INode& node);
  Result<KodaExpr> buildWithinExpr(const IFlow& flow, const INode& node);
  Result<QList<KodaExpr>> buildHandlers(const IFlow& flow, const INode& node);

  QList<NodeTransition> sequentialPredecessorsOf(const INode& node, const IFlow& flow) const;
  QSet<QString> sequentiallyReachableFrom(const INode& node, const IFlow& flow) const;

  int maxSequentialDistanceFromBranches(const IFlow& flow, const QList<NodeTransition>& branches, const INode& target) const;
  int sequentialDistanceBetween(const IFlow& flow, const INode& start, const INode& target) const;

  const INode* findStartNode(const IFlow& flow) const;
  const INode* findDestination(const QString& dstId, const IFlow& flow) const;

  QSet<QString> reachableFrom(const INode& node, const IFlow& flow) const;
  const INode* findNearestCommonJoin(const IFlow& flow, const QList<NodeTransition>& branches) const;

  QString emitExpr(const KodaExpr& expr, const QString& indent) const;
  QString emitNodeCall(const INode& node) const;

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
};

}  // namespace koda
