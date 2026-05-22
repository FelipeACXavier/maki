#include "maki_to_koda.h"

#include <QQueue>
#include <QStringList>
#include <QVariant>
#include <limits>

#include "logging.h"

namespace koda
{

Result<QString> MakiToKoda::generateFlow(const INode& owner, const IFlow& flow, const QString& indent)
{
  Q_UNUSED(owner);

  auto ast = buildFlowAst(flow);
  if (!ast)
    return Result<QString>::Failed(ast.ErrorMessage());

  return emitExpr(ast.Value(), indent);
}

Result<KodaExpr> MakiToKoda::buildFlowAst(const IFlow& flow)
{
  LOG_DEBUG("Building flow AST for %s", qPrintable(flow.getname()));

  const auto* start = findStartNode(flow);
  if (start == nullptr)
    return Result<KodaExpr>::Failed("Flow has no Koda::Start node");

  return buildSequenceFrom(flow, start, nullptr);
}

Result<KodaExpr> MakiToKoda::buildSequenceFrom(const IFlow& flow, const INode* start, const INode* stop)
{
  KodaExpr sequence;
  sequence.kind = KodaExpr::Kind::Sequence;

  const INode* current = start;
  QSet<QString> visited;

  while (current != nullptr && current != stop)
  {
    if (visited.contains(current->getid()))
    {
      return Result<KodaExpr>::Failed(
          "Cycle detected while building Koda AST at node: " + current->getid().toStdString());
    }

    visited.insert(current->getid());

    if (isEndNode(*current))
    {
      sequence.children.append(KodaExpr{KodaExpr::Kind::End});
      break;
    }

    if (!isStructuralNode(*current))
    {
      auto nodeExpr = buildNodeExpr(flow, *current);
      if (!nodeExpr)
        return Result<KodaExpr>::Failed(nodeExpr.ErrorMessage());

      sequence.children.append(nodeExpr.Value());
    }

    const auto normalSuccessors = sequentialSuccessorsOf(*current, flow);

    if (normalSuccessors.size() > 1)
    {
      const INode* joinNode = nullptr;

      auto joinExpr = buildJoinFromFanOut(flow, *current, normalSuccessors, joinNode);
      if (!joinExpr)
        return Result<KodaExpr>::Failed(joinExpr.ErrorMessage());

      sequence.children.append(joinExpr.Value());

      if (joinNode == nullptr)
        return Result<KodaExpr>::Failed("Internal error: join node was not resolved");

      const auto afterJoin = sequentialSuccessorsOf(*joinNode, flow);

      if (afterJoin.size() > 1)
      {
        return Result<KodaExpr>::Failed(
            "Join node has multiple sequential outgoing transitions: " + joinNode->getid().toStdString());
      }

      current = afterJoin.isEmpty() ? nullptr : afterJoin.first().node;
      continue;
    }

    current = normalSuccessors.isEmpty() ? nullptr : normalSuccessors.first().node;
  }

  return sequence;
}

Result<KodaExpr> MakiToKoda::buildNodeExpr(const IFlow& flow, const INode& node)
{
  if (node.getnodeId() == "Koda::Within")
    return buildWithinExpr(flow, node);

  KodaExpr call;
  call.kind = KodaExpr::Kind::Call;
  call.text = emitNodeCall(node);

  auto handlers = buildHandlers(flow, node);
  if (!handlers)
    return Result<KodaExpr>::Failed(handlers.ErrorMessage());

  call.children.append(handlers.Value());

  return call;
}

Result<KodaExpr> MakiToKoda::buildWithinExpr(const IFlow& flow, const INode& node)
{
  KodaExpr expr;
  expr.kind = KodaExpr::Kind::Call;
  expr.text = "within";

  const auto doSuccessors = doSuccessorsOf(node, flow);
  const auto elseSuccessors = elseSuccessorsOf(node, flow);

  if (doSuccessors.size() != 1)
    return Result<KodaExpr>::Failed("Within node must have exactly one 'do' transition: " + node.getid().toStdString());

  if (elseSuccessors.size() > 1)
    return Result<KodaExpr>::Failed("Within node cannot have more than one 'else' transition: " + node.getid().toStdString());

  auto doSequence = buildSequenceFrom(flow, doSuccessors.first().node, nullptr);
  if (!doSequence)
    return Result<KodaExpr>::Failed(doSequence.ErrorMessage());

  KodaExpr doBranch;
  doBranch.kind = KodaExpr::Kind::Branch;
  doBranch.text = "do";
  doBranch.children.append(doSequence.Value());

  expr.children.append(doBranch);

  if (!elseSuccessors.isEmpty())
  {
    auto elseSequence = buildSequenceFrom(flow, elseSuccessors.first().node, nullptr);
    if (!elseSequence)
      return Result<KodaExpr>::Failed(elseSequence.ErrorMessage());

    KodaExpr elseBranch;
    elseBranch.kind = KodaExpr::Kind::Branch;
    elseBranch.text = "else";
    elseBranch.children.append(elseSequence.Value());

    expr.children.append(elseBranch);
  }

  auto handlers = buildHandlers(flow, node);
  if (!handlers)
    return Result<KodaExpr>::Failed(handlers.ErrorMessage());

  expr.children.append(handlers.Value());

  return expr;
}

Result<QList<KodaExpr>> MakiToKoda::buildHandlers(const IFlow& flow, const INode& node)
{
  QList<KodaExpr> handlers;

  for (const auto& errorStart : errorSuccessorsOf(node, flow))
  {
    auto sequence = buildSequenceFrom(flow, errorStart.node, nullptr);
    if (!sequence)
      return Result<QList<KodaExpr>>::Failed(sequence.ErrorMessage());

    KodaExpr handler;
    handler.kind = KodaExpr::Kind::Handler;
    handler.text = "on error";
    handler.children.append(sequence.Value());

    handlers.append(handler);
  }

  for (const auto& abortStart : abortSuccessorsOf(node, flow))
  {
    auto sequence = buildSequenceFrom(flow, abortStart.node, nullptr);
    if (!sequence)
      return Result<QList<KodaExpr>>::Failed(sequence.ErrorMessage());

    KodaExpr handler;
    handler.kind = KodaExpr::Kind::Handler;
    handler.text = "on abort";
    handler.children.append(sequence.Value());

    handlers.append(handler);
  }

  for (const auto& signalStart : signalSuccessorsOf(node, flow))
  {
    auto sequence = buildSequenceFrom(flow, signalStart.node, nullptr);
    if (!sequence)
      return Result<QList<KodaExpr>>::Failed(sequence.ErrorMessage());

    KodaExpr handler;
    handler.kind = KodaExpr::Kind::Handler;
    handler.text = "on " + signalStart.transition->getevent();
    handler.children.append(sequence.Value());

    handlers.append(handler);
  }

  return handlers;
}

Result<KodaExpr> MakiToKoda::buildJoinFromFanOut(const IFlow& flow, const INode& splitNode, const QList<NodeTransition>& successors, const INode*& joinNode)
{
  joinNode = findNearestCommonJoin(flow, successors);

  if (joinNode == nullptr)
  {
    return Result<KodaExpr>::Failed(
        "Could not find common join node for sequential fan-out after node: " + splitNode.getid().toStdString());
  }

  KodaExpr join;
  join.kind = KodaExpr::Kind::Join;

  for (const auto& branchStart : successors)
  {
    auto branch = buildSequenceFrom(flow, branchStart.node, joinNode);
    if (!branch)
      return Result<KodaExpr>::Failed(branch.ErrorMessage());

    join.children.append(branch.Value());
  }

  return join;
}

const INode* MakiToKoda::findStartNode(const IFlow& flow) const
{
  for (const auto& node : flow.getnodes())
  {
    if (node->getnodeId() == "Koda::Start")
      return node.get();
  }

  return nullptr;
}

const INode* MakiToKoda::findDestination(const QString& dstId, const IFlow& flow) const
{
  for (const auto& node : flow.getnodes())
  {
    if (node->getid() == dstId)
      return node.get();
  }

  return nullptr;
}

QList<NodeTransition> MakiToKoda::sequentialSuccessorsOf(const INode& node, const IFlow& flow) const
{
  return successorsOfKind(node, flow, TransitionKind::Sequential);
}

QList<NodeTransition> MakiToKoda::errorSuccessorsOf(const INode& node, const IFlow& flow) const
{
  return successorsOfKind(node, flow, TransitionKind::Error);
}

QList<NodeTransition> MakiToKoda::abortSuccessorsOf(const INode& node, const IFlow& flow) const
{
  return successorsOfKind(node, flow, TransitionKind::Abort);
}

QList<NodeTransition> MakiToKoda::doSuccessorsOf(const INode& node, const IFlow& flow) const
{
  return successorsOfKind(node, flow, TransitionKind::Do);
}

QList<NodeTransition> MakiToKoda::elseSuccessorsOf(const INode& node, const IFlow& flow) const
{
  return successorsOfKind(node, flow, TransitionKind::Else);
}

QList<NodeTransition> MakiToKoda::signalSuccessorsOf(const INode& node, const IFlow& flow) const
{
  return successorsOfKind(node, flow, TransitionKind::Signal);
}

QList<NodeTransition> MakiToKoda::sequentialPredecessorsOf(const INode& node, const IFlow& flow) const
{
  QList<NodeTransition> result;

  for (const auto& candidate : flow.getnodes())
  {
    for (const auto& transition : flow.gettransitions(candidate->getid()))
    {
      if (!isSequentialTransition(*transition))
        continue;

      if (transition->getdstId() == node.getid())
      {
        result.append({candidate.get(), transition.get()});
        break;
      }
    }
  }

  return result;
}

QSet<QString> MakiToKoda::sequentiallyReachableFrom(const INode& node, const IFlow& flow) const
{
  QSet<QString> visited;
  QQueue<const INode*> queue;

  queue.push_back(&node);

  while (!queue.isEmpty())
  {
    const auto* current = queue.front();
    queue.pop_front();

    if (current == nullptr)
      continue;

    if (visited.contains(current->getid()))
      continue;

    visited.insert(current->getid());

    for (const auto& successor : sequentialSuccessorsOf(*current, flow))
      queue.push_back(successor.node);
  }

  return visited;
}

const INode* MakiToKoda::findNearestCommonJoin(const IFlow& flow, const QList<NodeTransition>& branches) const
{
  if (branches.isEmpty())
    return nullptr;

  QList<QSet<QString>> reachableSets;

  for (const auto& branch : branches)
  {
    if (branch.node == nullptr)
      return nullptr;

    reachableSets.append(sequentiallyReachableFrom(*branch.node, flow));
  }

  QSet<QString> common = reachableSets.first();

  for (const auto& reachable : reachableSets)
    common.intersect(reachable);

  const INode* best = nullptr;
  int bestDistance = std::numeric_limits<int>::max();

  for (const auto& candidateNode : flow.getnodes())
  {
    const auto* candidate = candidateNode.get();

    if (candidate == nullptr)
      continue;

    if (!common.contains(candidate->getid()))
      continue;

    if (sequentialPredecessorsOf(*candidate, flow).size() < 2)
      continue;

    const int distance = maxSequentialDistanceFromBranches(flow, branches, *candidate);

    if (distance >= 0 && distance < bestDistance)
    {
      best = candidate;
      bestDistance = distance;
    }
  }

  return best;
}

int MakiToKoda::maxSequentialDistanceFromBranches(const IFlow& flow, const QList<NodeTransition>& branches, const INode& target) const
{
  int maxDistance = 0;

  for (const auto& branch : branches)
  {
    const int distance = sequentialDistanceBetween(flow, *branch.node, target);

    if (distance < 0)
      return -1;

    maxDistance = std::max(maxDistance, distance);
  }

  return maxDistance;
}

int MakiToKoda::sequentialDistanceBetween(const IFlow& flow, const INode& start, const INode& target) const
{
  struct QueueItem
  {
    const INode* node = nullptr;
    int distance = 0;
  };

  QSet<QString> visited;
  QQueue<QueueItem> queue;

  queue.push_back(QueueItem{&start, 0});

  while (!queue.isEmpty())
  {
    const auto item = queue.front();
    queue.pop_front();

    if (item.node == nullptr)
      continue;

    if (visited.contains(item.node->getid()))
      continue;

    visited.insert(item.node->getid());

    if (item.node->getid() == target.getid())
      return item.distance;

    for (const auto& successor : sequentialSuccessorsOf(*item.node, flow))
      queue.push_back(QueueItem{successor.node, item.distance + 1});
  }

  return -1;
}

QString MakiToKoda::emitExpr(const KodaExpr& expr, const QString& indent) const
{
  Q_UNUSED(indent);

  switch (expr.kind)
  {
    case KodaExpr::Kind::Empty:
      return "";

    case KodaExpr::Kind::Call:
    {
      QString result = expr.text;
      for (const auto& child : expr.children)
        if (child.kind == KodaExpr::Kind::Branch || child.kind == KodaExpr::Kind::Handler)
          result += " " + emitExpr(child, indent);

      return result;
    }

    case KodaExpr::Kind::Branch:
    {
      if (expr.children.isEmpty())
        return expr.text;

      return expr.text + " (" + emitExpr(expr.children.first(), indent) + ")";
    }

    case KodaExpr::Kind::Handler:
    {
      if (expr.children.isEmpty())
        return expr.text;

      return expr.text + " (" + emitExpr(expr.children.first(), indent) + ")";
    }

    case KodaExpr::Kind::End:
      return "end";

    case KodaExpr::Kind::Sequence:
    {
      QStringList parts;

      for (const auto& child : expr.children)
      {
        const auto emitted = emitExpr(child, indent);
        if (!emitted.isEmpty())
          parts << emitted;
      }

      return parts.join(" -> ");
    }

    case KodaExpr::Kind::Join:
    {
      QStringList branches;

      for (const auto& child : expr.children)
      {
        const auto emitted = emitExpr(child, indent);
        if (!emitted.isEmpty())
          branches << emitted;
      }

      return "join(" + branches.join(" | ") + ")";
    }
  }

  return "";
}

QString MakiToKoda::emitNodeCall(const INode& node) const
{
  const QString type = node.getnodeId();
  const auto properties = node.getproperties();

  QString name = properties.value("name").toString();

  if (name.isEmpty())
    name = properties.value("Name").toString();

  if (type == "Koda::Async task")
    return name + "()";

  if (type == "Koda::Sync task")
    return name + "()";

  if (type == "Koda::Strategy")
    return name + "()";

  if (type == "Koda::Success")
    return "success";

  if (type == "Koda::Error")
    return "error";

  if (type == "Koda::Continue")
    return "continue";

  if (type == "Koda::Within")
    return "within(" + name + ")";

  if (type == "Koda::Repeat")
    return "repeat(" + name + ")";

  if (type == "Koda::Every")
    return "every(" + name + ")";

  return name.isEmpty() ? type : name + "()";
}

bool MakiToKoda::isEndNode(const INode& node) const
{
  return node.getnodeId() == "Koda::End";
}

bool MakiToKoda::isStructuralNode(const INode& node) const
{
  return node.getnodeId() == "Koda::Start";
}

QList<NodeTransition> MakiToKoda::successorsOfKind(const INode& node, const IFlow& flow, TransitionKind kind) const
{
  QList<NodeTransition> result;

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transitionKind(*transition) != kind)
      continue;

    const auto* dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
      result.append(NodeTransition{dst, transition.get()});
  }

  return result;
}

bool MakiToKoda::isSequentialTransition(const ITransition& transition) const
{
  return transitionKind(transition) == TransitionKind::Sequential;
}

bool MakiToKoda::isErrorTransition(const ITransition& transition) const
{
  return transitionKind(transition) == TransitionKind::Error;
}

bool MakiToKoda::isAbortTransition(const ITransition& transition) const
{
  return transitionKind(transition) == TransitionKind::Abort;
}

bool MakiToKoda::isDoTransition(const ITransition& transition) const
{
  return transitionKind(transition) == TransitionKind::Do;
}

bool MakiToKoda::isElseTransition(const ITransition& transition) const
{
  return transitionKind(transition) == TransitionKind::Else;
}

bool MakiToKoda::isSignalTransition(const ITransition& transition) const
{
  return transitionKind(transition) == TransitionKind::Signal;
}

TransitionKind MakiToKoda::transitionKind(const ITransition& transition) const
{
  const auto label = transition.getlabel().trimmed();

  if (label.isEmpty())
    return TransitionKind::Sequential;
  else if (label == "on error")
    return TransitionKind::Error;
  else if (label == "on abort")
    return TransitionKind::Abort;
  else if (label == "do")
    return TransitionKind::Do;
  else if (label == "else")
    return TransitionKind::Else;
  else if (label.contains("on"))
    return TransitionKind::Signal;

  return TransitionKind::Unknown;
}

}  // namespace koda
