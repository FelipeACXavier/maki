#include "maki_to_koda.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QQueue>
#include <QStringList>
#include <QVariant>
#include <any>
#include <limits>
#include <memory>
#include <vector>

#include "ast/ast.h"
#include "logging.h"
#include "result.h"
#include "string_helpers.h"
#include "types.h"

#include <iomanip>
#include <functional>

#ifdef USE_ANTLR
#include "koda_emitter.h"
#endif

namespace koda
{

const int INVALID_INTEGER = std::numeric_limits<int>::max();
const double INVALID_DOUBLE = std::numeric_limits<double>::max();

std::string MakiToKoda::generateUniqueId()
{
  // Generate hash from counter
  std::hash<uint32_t> hasher;
  size_t hashValue = hasher(++mIdCounter);

  // Convert to 4-character hex (16-bit, compact and readable)
  std::ostringstream oss;
  oss << std::hex << std::setfill('0') << std::setw(4) << (hashValue & 0xFFFF);
  return oss.str();
}

Result<QString> MakiToKoda::generate(const QVector<std::shared_ptr<INode>> nodes)
{
  koda::System sys;

  for (const auto& node : nodes)
  {
    auto taskAny = buildTask(*node);
    if (!taskAny)
      return Result<QString>::Failed("Failed to generate task: " + taskAny.ErrorMessage());

    for (const auto& child : node->getchildren())
    {
      auto generated = buildCapability(*child);
      if (!generated)
        return Result<QString>::Failed("Failed to generate capability: " + generated.ErrorMessage());

      sys.components.push_back(generated.Value());
    }

    sys.components.push_back(taskAny.Value());
  }
#ifdef USE_ANTLR
  auto contents = KodaEmitter::emitKoda(sys);
  RETURN_ON_FAILURE_AS(contents, QString);
  return QString::fromStdString(contents.Value());
#else
  return QString();
#endif
}

Result<koda::PComponent> MakiToKoda::buildTask(const INode& task)
{
  auto c = std::make_shared<koda::Component>();
  c->kind = koda::Component::Kind::Task;
  auto properties = task.getproperties();
  if (!properties.contains("name"))
    return Result<koda::PComponent>::Failed("Task does not have a name");

  auto name = properties["name"].toString().toLower();
  std::string formattedName = format(name, "_");
  std::string uniqueId = generateUniqueId();
  c->name = formattedName + "_" + uniqueId;

  LOG_DEBUG("Generated Task: %s", c->name.c_str());

  // Get arguments
  for (const auto& cap : task.getchildren())
  {
    auto capName = cap->getproperties()["name"].toString();

    auto parg = std::make_shared<koda::Argument>();
    parg->kind = koda::Argument::Kind::Req;
    parg->a = format(capName);
    ToLowerCase(parg->a, 0);
    parg->b = format(capName);

    c->args.push_back(parg);
  }

  // Get events
  for (const auto& event : task.getevents())
  {
    auto rosDef = buildRosDef(*event);
    RETURN_ON_FAILURE_AS(rosDef, koda::PComponent);

    auto statement = std::make_shared<koda::Statement>();
    statement->node = rosDef.Value();
    c->statements.push_back(statement);
  }

  // Get variables
  auto varsBlock = std::make_shared<koda::VarsBlock>();
  for (const auto& child : task.getfields())
  {
    auto generated = buildVarDef(*child);
    RETURN_ON_FAILURE_AS(generated, koda::PComponent);
    varsBlock->vars.push_back(generated.Value());
  }

  auto varsStatement = std::make_shared<koda::Statement>();
  varsStatement->node = varsBlock;
  c->statements.push_back(varsStatement);

  // Get flows
  auto strategyBlock = std::make_shared<koda::StrategyBlock>();
  for (const auto& flow : task.getflows())
  {
    auto generated = buildFlowAst(*flow);
    RETURN_ON_FAILURE_AS(generated, koda::PComponent);
    strategyBlock->flows.push_back(generated.Value());
  }

  auto strategyStatement = std::make_shared<koda::Statement>();
  strategyStatement->node = strategyBlock;
  c->statements.push_back(strategyStatement);

  return c;
}

Result<koda::PComponent> MakiToKoda::buildCapability(const INode& capability)
{
  auto c = std::make_shared<koda::Component>();
  c->kind = koda::Component::Kind::Capability;
  auto properties = capability.getproperties();
  if (!properties.contains("name"))
    return Result<koda::PComponent>::Failed("Capabiity does not have a name");

  auto name = properties["name"].toString();
  std::string formattedName = format(name);
  std::string uniqueID = generateUniqueId();
  c->name = formattedName + "_" + uniqueID;

  auto typeArray = properties["type"].toJsonObject()["options"].toArray();
  if (typeArray.isEmpty())
    return Result<koda::PComponent>::Failed("Type options is empty: " + c->name);

  auto actions = buildActionDefs(capability, typeArray);
  RETURN_ON_FAILURE_AS(actions, koda::PComponent);
  for (const auto& action : actions.Value())
  {
    auto statement = std::make_shared<koda::Statement>();
    statement->node = action;
    c->statements.push_back(statement);
  }

  return c;
}

Result<koda::PVarDef> MakiToKoda::buildVarDef(const IProperty& property)
{
  auto varDef = std::make_shared<koda::VarDef>();
  varDef->name = property.getid().toStdString();
  varDef->varType = Types::PropertyTypesToString(property.gettype()).toStdString();

  auto wrapper = std::make_shared<koda::Expr>();
  switch (property.gettype())
  {
    case Types::PropertyTypes::BOOLEAN:
    case Types::PropertyTypes::INTEGER:
    {
      auto expr = std::make_shared<koda::Expr::Int>();
      expr->value = property.getdefaultValue().toInt();
      wrapper->v = expr;
      break;
    }
    case Types::PropertyTypes::REAL:
    {
      auto expr = std::make_shared<koda::Expr::Float>();
      expr->value = property.getdefaultValue().toDouble();
      wrapper->v = expr;
      break;
    }
    case Types::PropertyTypes::STRING:
    {
      auto expr = std::make_shared<koda::Expr::Str>();
      expr->value = property.getdefaultValue().toString().toStdString();
      wrapper->v = expr;
      break;
    }
    default:
    {
      auto expr = std::make_shared<koda::Expr::Id>();
      expr->value = "";
      wrapper->v = expr;
      break;
    }
  }
  varDef->init = wrapper;

  return varDef;
}

Result<std::vector<koda::PActionDef>> MakiToKoda::buildActionDefs(const INode& node, const QJsonArray& typeArray)
{
  std::vector<koda::PActionDef> actions;

  for (int i = 0; i < typeArray.size(); ++i)
  {
    const auto item = typeArray.at(i).toObject();
    auto callType = item["id"].toString();
    auto callOptions = item["options"].toArray();
    if (callOptions.size() < 2)
      return Result<std::vector<koda::PActionDef>>::Failed("Action definition does follow the expected format: " + callType.toStdString());

    auto callRoute = callOptions.at(0).toObject()["default"].toString();
    auto callMessage = callOptions.at(1).toObject()["default"].toString();

    auto action = std::make_shared<koda::ActionDef>();
    if (callType.trimmed() == "action")
      action->kind = koda::ActionDef::Kind::Action;
    else if (callType.trimmed() == "service")
      action->kind = koda::ActionDef::Kind::Service;
    else if (callType.trimmed() == "topic")
      action->kind = koda::ActionDef::Kind::Topic;
    else
      return Result<std::vector<koda::PActionDef>>::Failed("Unknown action definition type: " + callType.trimmed().toStdString());

    action->label1 = callRoute.toStdString();
    action->label2 = callMessage.toStdString();

    for (const auto& event : node.getevents())
    {
      if (event->getlinksTo() != i)
        continue;

      auto rosDef = buildRosDef(*event);
      RETURN_ON_FAILURE_AS(rosDef, std::vector<koda::PActionDef>);
      action->rosDefs.push_back(rosDef.Value());
    }

    actions.push_back(action);
  }

  return actions;
}

Result<koda::PRosDef> MakiToKoda::buildRosDef(const IFlow& event)
{
  auto def = std::make_shared<koda::RosDef>();
  if (event.gettype() == Types::CallType::TRIGGER)
    def->kind = koda::RosDef::Kind::Trigger;
  else if (event.gettype() == Types::CallType::ABORT)
    def->kind = koda::RosDef::Kind::Abort;
  else if (event.gettype() == Types::CallType::RETURN)
    def->kind = koda::RosDef::Kind::Return;
  else if (event.gettype() == Types::CallType::ERROR)
    def->kind = koda::RosDef::Kind::Error;
  else if (event.gettype() == Types::CallType::IN)
    def->kind = koda::RosDef::Kind::In;
  else if (event.gettype() == Types::CallType::OUT)
    def->kind = koda::RosDef::Kind::Out;
  else
    return Result<koda::PRosDef>::Failed("Unknown call type");

  auto eventDef = std::make_shared<koda::EventDef>();
  eventDef->typeName = Types::PropertyTypesToString(event.getreturnType()).toStdString();
  eventDef->name = event.getname().toStdString();

  for (const auto& arg : event.getarguments())
  {
    auto parg = std::make_shared<koda::Argument>();
    parg->kind = koda::Argument::Kind::Plain;
    parg->a = Types::PropertyTypesToString(arg->gettype()).toStdString();
    parg->b = arg->getid().toStdString();
    eventDef->args.push_back(parg);
  }

  def->def = eventDef;

  return def;
}

Result<koda::PFlow> MakiToKoda::buildFlowAst(const IFlow& flow)
{
  LOG_DEBUG("Building flow AST for %s", qPrintable(flow.getname()));

  const auto* start = findStartNode(flow);
  if (start == nullptr)
    return Result<koda::PFlow>::Failed("Flow has no Koda::Start node");

  // Prepare all the variables for this flow
  for (const auto& arg : flow.getarguments())
    mVariables.push_back(arg->getid().toStdString());

  auto seq = buildSequenceFrom(flow, start, nullptr);
  if (!seq.has_value())
    return Result<koda::PFlow>::Failed("Failed to build first sequence");

  auto pflow = std::make_shared<koda::Flow>();
  auto flowName = flow.getname();
  if (flowName != "main")
    flowName = "f" + flowName;

  pflow->name = format(flowName);
  pflow->strategy = std::any_cast<koda::PStrategy>(seq);
  for (const auto& arg : flow.getarguments())
    pflow->tags.push_back(arg->getid().toStdString());

  // Clear variables at the end of the flow
  mVariables.clear();

  return pflow;
}

std::any MakiToKoda::buildSequenceFrom(const IFlow& flow, const INode* start, const INode* stop)
{
  auto sequence = std::make_shared<koda::Strategy::Seq>();

  const INode* current = start;
  QSet<QString> visited;

  while (current != nullptr && current != stop)
  {
    if (visited.contains(current->getid()))
    {
      LOG_ERROR("Cycle detected while building Koda AST at node: " + current->getid().toStdString());
      return std::any();
    }

    visited.insert(current->getid());

    if (isEndNode(*current))
    {
      auto value = std::make_shared<koda::Strategy::End>();
      auto node = std::make_shared<koda::Strategy>();
      node->v = value;
      sequence->alts.push_back(node);
      break;
    }

    if (!isStructuralNode(*current))
    {
      auto nodeExpr = buildNodeExpr(flow, *current);
      if (!nodeExpr.has_value())
      {
        LOG_ERROR("Could not build non structural node");
        return std::any();
      }

      sequence->alts.push_back(std::any_cast<koda::PStrategy>(nodeExpr));
    }

    const auto normalSuccessors = sequentialSuccessorsOf(*current, flow);

    if (normalSuccessors.size() > 1)
    {
      const INode* joinNode = nullptr;

      auto joinExpr = buildJoinFromFanOut(flow, *current, normalSuccessors, joinNode);
      if (!joinExpr.has_value())
      {
        LOG_ERROR("Failed to build join expression");
        return std::any();
      }

      if (joinNode == nullptr)
      {
        LOG_ERROR("Internal error: join node was not resolved");
        return std::any();
      }

      sequence->alts.push_back(std::any_cast<koda::PStrategy>(joinExpr));
      const auto afterJoin = sequentialSuccessorsOf(*joinNode, flow);

      if (afterJoin.size() > 1)
      {
        LOG_ERROR("Join node has multiple sequential outgoing transitions: " + joinNode->getid().toStdString());
        return std::any();
      }

      current = afterJoin.isEmpty() ? nullptr : afterJoin.first().node;
      continue;
    }

    current = normalSuccessors.isEmpty() ? nullptr : normalSuccessors.first().node;
  }

  if (sequence->alts.size() == 1)
    return sequence->alts.front();

  auto node = std::make_shared<koda::Strategy>();
  node->v = sequence;

  return node;
}

std::any MakiToKoda::buildNodeExpr(const IFlow& flow, const INode& node)
{
  if (node.getnodeId() == "Koda::Async task")
    return buildAsyncExpr(flow, node);
  else if (node.getnodeId() == "Koda::Sync task")
    return buildSyncExpr(flow, node);
  else if (node.getnodeId() == "Koda::Flow call")
    return buildStrategyExpr(flow, node);
  else if (node.getnodeId() == "Koda::Within")
    return buildWithinExpr(flow, node);
  else if (node.getnodeId() == "Koda::Repeat")
    return buildRepeatExpr(flow, node);
  else if (node.getnodeId() == "Koda::Continue")
    return buildContinueExpr(flow, node);
  else if (node.getnodeId() == "Koda::Terminate")
    return buildSuccessExpr(flow, node);

  LOG_ERROR("Unknown expression: %s", qPrintable(node.getnodeId()));
  return std::any();
}

std::any MakiToKoda::buildAsyncExpr(const IFlow& flow, const INode& node)
{
  QJsonObject object = node.getproperties()["capability"].toJsonObject();
  QString val = object["data"].toString();
  if (val.isEmpty())
  {
    LOG_ERROR("AsyncTask component does not have a valid capability");
    return std::any();
  }
  QJsonArray options = object["options"].toArray();
  if (val.isEmpty())
  {
    LOG_ERROR("AsyncTask component is missing an associated call");
    return std::any();
  }

  auto task = std::make_shared<koda::EventCall>();
  task->receiver = format(val);
  ToLowerCase(task->receiver, 0);

  auto expr = std::make_shared<koda::Strategy::TaskCall>();
  expr->call = task;
  task->args = buildArgumentExpr(options, 1);

  auto handlers = buildHandlers(flow, node);
  for (const auto& handler : handlers)
    expr->handlers.push_back(handler);

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;
  return strat;
}

std::any MakiToKoda::buildSyncExpr(const IFlow& flow, const INode& node)
{
  QJsonObject object = node.getproperties()["capability"].toJsonObject();
  QString val = object["data"].toString();
  if (val.isEmpty())
  {
    LOG_ERROR("AsyncTask component does not have a valid capability");
    return std::any();
  }
  QJsonArray options = object["options"].toArray();
  if (val.isEmpty())
  {
    LOG_ERROR("SyncTask component is missing an associated call");
    return std::any();
  }
  const auto method = options.at(0).toObject();

  auto task = std::make_shared<koda::EventCall>();
  task->receiver = format(val);
  ToLowerCase(task->receiver, 0);

  task->name = method["data"].toString().toStdString();
  task->args = buildArgumentExpr(options, 1);

  auto expr = std::make_shared<koda::Strategy::TaskCall>();
  expr->call = task;

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;
  return strat;
}

std::any MakiToKoda::buildStrategyExpr(const IFlow& flow, const INode& node)
{
  QJsonObject object = node.getproperties()["task"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();
  if (options.isEmpty())
  {
    LOG_ERROR("Strategy component does not have a valid flow");
    return std::any();
  }

  auto expr = std::make_shared<koda::Strategy::Ref>();
  expr->name = "f" + format(options[0].toObject()["data"].toString());

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;
  return strat;
}

std::any MakiToKoda::buildWithinExpr(const IFlow& flow, const INode& node)
{
  auto expr = std::make_shared<koda::Strategy::Within>();

  const auto doSuccessors = doSuccessorsOf(node, flow);
  const auto elseSuccessors = elseSuccessorsOf(node, flow);

  if (doSuccessors.size() != 1)
  {
    LOG_ERROR("Within node must have exactly one 'do' transition: " + node.getid().toStdString());
    return std::any();
  }

  if (elseSuccessors.size() > 1)
  {
    LOG_ERROR("Within node cannot have more than one 'else' transition: " + node.getid().toStdString());
    return std::any();
  }

  auto doSequence = buildSequenceFrom(flow, doSuccessors.first().node, nullptr);
  if (!doSequence.has_value())
  {
    LOG_ERROR("Failed to create do sequence");
    return std::any();
  }

  expr->a = std::any_cast<koda::PStrategy>(doSequence);

  if (!elseSuccessors.isEmpty())
  {
    auto elseSequence = buildSequenceFrom(flow, elseSuccessors.first().node, nullptr);
    if (!elseSequence.has_value())
    {
      LOG_ERROR("Failed to create else sequence");
      return std::any();
    }

    expr->b = std::any_cast<koda::PStrategy>(elseSequence);
  }

  auto properties = node.getproperties();
  if (!properties.contains("timeout"))
  {
    LOG_ERROR("Within missing timeout property");
    return std::any();
  }
  expr->seconds = properties["timeout"].toInt();

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;

  return strat;
}

std::any MakiToKoda::buildRepeatExpr(const IFlow& flow, const INode& node)
{
  auto properties = node.getproperties();
  if (!properties.contains("capability"))
  {
    LOG_ERROR("Repeat component does not have an associated flow");
    return std::any();
  }
  if (!properties.contains("iterations"))
  {
    LOG_ERROR("Repeat component is missing the iterations property");
    return std::any();
  }
  if (!properties.contains("rate"))
  {
    LOG_ERROR("Repeat component is missing the rate property");
    return std::any();
  }

  QJsonObject object = properties["capability"].toJsonObject();
  QJsonArray options = object["options"].toArray();
  if (options.isEmpty())
  {
    LOG_ERROR("Repeat component does not have an associated flow");
    return std::any();
  }

  auto expr = std::make_shared<koda::Strategy::Repeat>();
  expr->iterations = properties["iterations"].toInt();
  expr->seconds = properties["rate"].toInt();

  auto strategy = format(options[0].toObject()["data"].toString());

  auto ref = std::make_shared<koda::Strategy::Ref>();
  ref->name = "f" + strategy;

  auto repeatStrat = std::make_shared<koda::Strategy>();
  repeatStrat->v = ref;
  expr->a = repeatStrat;

  const auto seqSuccessors = sequentialSuccessorsOf(node, flow);
  if (!seqSuccessors.empty())
  {
    auto sequence = buildSequenceFrom(flow, seqSuccessors.first().node, nullptr);
    if (!sequence.has_value())
    {
      LOG_ERROR("Failed to create do sequence");
      return std::any();
    }
  }

  auto handlers = buildHandlers(flow, node);
  for (const auto& handler : handlers)
    expr->handlers.push_back(handler);

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;
  return strat;
}

std::any MakiToKoda::buildContinueExpr(const IFlow& flow, const INode& node)
{
  auto expr = std::make_shared<koda::Strategy::Continue>();
  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;

  return strat;
}

std::any MakiToKoda::buildSuccessExpr(const IFlow& flow, const INode& node)
{
  auto expr = std::make_shared<koda::Strategy::End>();
  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;

  return strat;
}

QList<koda::PStrategyHandler> MakiToKoda::buildHandlers(const IFlow& flow, const INode& node)
{
  QList<koda::PStrategyHandler> handlers;

  for (const auto& errorStart : errorSuccessorsOf(node, flow))
  {
    auto sequence = buildSequenceFrom(flow, errorStart.node, nullptr);
    if (!sequence.has_value())
    {
      LOG_ERROR("Failed to parse error handler");
      return {};
    }

    auto value = std::make_shared<koda::StrategyHandler>();
    value->kind = koda::StrategyHandler::Kind::OnError;
    value->body = std::any_cast<koda::PStrategy>(sequence);

    handlers.append(value);
  }

  for (const auto& abortStart : abortSuccessorsOf(node, flow))
  {
    auto sequence = buildSequenceFrom(flow, abortStart.node, nullptr);
    if (!sequence.has_value())
    {
      LOG_ERROR("Failed to parse abort handler");
      return {};
    }

    auto value = std::make_shared<koda::StrategyHandler>();
    value->kind = koda::StrategyHandler::Kind::OnAbort;
    value->body = std::any_cast<koda::PStrategy>(sequence);
    handlers.append(value);
  }

  for (const auto& signalStart : signalSuccessorsOf(node, flow))
  {
    auto sequence = buildSequenceFrom(flow, signalStart.node, nullptr);
    if (!sequence.has_value())
    {
      LOG_ERROR("Failed to parse signal handler");
      return {};
    }

    auto value = std::make_shared<koda::StrategyHandler>();
    value->kind = koda::StrategyHandler::Kind::OnEmitter;
    value->body = std::any_cast<koda::PStrategy>(sequence);

    auto emitter = std::make_shared<koda::EventCall>();
    auto event = format(signalStart.transition->getevent());
    auto receiverIndex = event.find_first_of('.');
    emitter->name = event.substr(receiverIndex + 1);
    emitter->receiver = event.substr(0, receiverIndex);
    ToLowerCase(emitter->receiver, 0);
    value->emitter = emitter;

    handlers.append(value);
  }

  return handlers;
}

std::shared_ptr<koda::Expr> MakiToKoda::buildExpr(const QJsonObject& object)
{
  if (!object.contains("type"))
  {
    LOG_ERROR("Expr with no type");
    return nullptr;
  }

  if (!object.contains("data"))
  {
    LOG_ERROR("Expr with no data");
    return nullptr;
  }

  auto type = Types::StringToPropertyTypes(object["type"].toString());
  auto wrapper = std::make_shared<koda::Expr>();

  // Before anything, lets check to see if we are dealing with a variable
  auto varValue = object["data"].toString().toStdString();
  if (std::count(mVariables.cbegin(), mVariables.cend(), varValue) > 0)
  {
    auto expr = std::make_shared<koda::Expr::Id>();
    expr->value = varValue;
    wrapper->v = expr;
    return wrapper;
  }

  // If not a variable, we can continue
  switch (type)
  {
    case Types::PropertyTypes::BOOLEAN:
    {
      auto expr = std::make_shared<koda::Expr::Int>();
      expr->value = (object["data"].toString().toLower() == "true");
      wrapper->v = expr;
      break;
    }
    case Types::PropertyTypes::INTEGER:
    {
      auto expr = std::make_shared<koda::Expr::Int>();
      expr->value = object["data"].toInt();
      wrapper->v = expr;
      break;
    }
    case Types::PropertyTypes::REAL:
    {
      auto expr = std::make_shared<koda::Expr::Float>();
      expr->value = object["data"].toString().toDouble();
      wrapper->v = expr;
      break;
    }
    case Types::PropertyTypes::STRING:
    {
      auto expr = std::make_shared<koda::Expr::Str>();
      expr->value = object["data"].toString().toStdString();
      wrapper->v = expr;
      break;
    }
    default:
    {
      auto expr = std::make_shared<koda::Expr::Id>();
      expr->value = object["data"].toString().toStdString();
      wrapper->v = expr;
      break;
    }
  }

  return wrapper;
}

std::vector<std::shared_ptr<Expr>> MakiToKoda::buildArgumentExpr(const QJsonArray& options, int start)
{
  std::vector<std::shared_ptr<Expr>> args = {};

  for (int i = start; i < options.size(); ++i)
  {
    const auto arg = options.at(i).toObject();
    auto parg = buildExpr(arg);
    if (parg != nullptr)
      args.push_back(parg);
  }

  return args;
}

std::any MakiToKoda::buildJoinFromFanOut(const IFlow& flow, const INode& splitNode, const QList<NodeTransition>& successors, const INode*& joinNode)
{
  joinNode = findNearestCommonJoin(flow, successors);

  if (joinNode == nullptr)
  {
    LOG_ERROR("Could not find common join node for sequential fan-out after node: " + splitNode.getid().toStdString());
    return std::any();
  }

  auto join = std::make_shared<koda::Strategy::Join>();

  for (const auto& branchStart : successors)
  {
    auto branch = buildSequenceFrom(flow, branchStart.node, joinNode);
    if (!branch.has_value())
      return std::any();

    join->alts.push_back(std::any_cast<koda::PStrategy>(branch));
  }

  auto node = std::make_shared<koda::Strategy>();
  node->v = join;

  return node;
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

bool MakiToKoda::isEndNode(const INode& node) const
{
  return node.getnodeId() == "Koda::Terminate";
}

bool MakiToKoda::isStructuralNode(const INode& node) const
{
  return node.getnodeId() == "Koda::Start" || node.getnodeId() == "Koda::Success" || node.getnodeId() == "Koda::Failure";
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

std::string MakiToKoda::format(QString input, const QString& token) const
{
  return input.replace(" ", token).toStdString();
}

}  // namespace koda
