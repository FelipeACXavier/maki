#include "maki_to_koda.h"

#include <qdir.h>
#include <qhashfunctions.h>
#include <qjsonobject.h>

#include <QJsonArray>
#include <QJsonObject>
#include <QQueue>
#include <QStringList>
#include <QVariant>
#include <any>
#include <limits>
#include <memory>

#include "ast/ast.h"
#include "keys.h"
#include "koda_emitter.h"
#include "logging.h"
#include "result.h"
#include "string_helpers.h"
#include "types.h"
#include "typing/helpers.h"
#include "typing/type_reference.h"

#define LOG_AND_FAIL(NODE_ID, FLOW_ID, M, ...)                           \
  do                                                                     \
  {                                                                      \
    LOG_ERROR(M, ##__VA_ARGS__);                                         \
    mErrorListener.addError(NODE_ID, FLOW_ID, Format(M, ##__VA_ARGS__)); \
    return std::any();                                                   \
  } while (false)

#define RETURN_FAIL(NODE_ID, RET, M, ...)                           \
  do                                                                \
  {                                                                 \
    mErrorListener.addError(NODE_ID, "", Format(M, ##__VA_ARGS__)); \
    return RET::Failed(M, ##__VA_ARGS__);                           \
  } while (false)

namespace koda
{

MakiToKoda::MakiToKoda(const koda::types::TypeRegistry* registry, std::shared_ptr<koda::TraceabilityMap> traceMap)
    : mTypeRegistry(registry)
    , mTraceMap(traceMap)
{
}

std::vector<MakiErrorListener::Error> MakiToKoda::getErrors() const
{
  return mErrorListener.mErrors;
}

koda::System MakiToKoda::getAST() const
{
  return mAST;
}

Result<QString> MakiToKoda::generate(const QVector<std::shared_ptr<INode>> nodes, QVector<const IParameter*> missionParameters)
{
  mAST = koda::System{};

  for (const auto& node : nodes)
  {
    for (const auto& child : node->getchildren())
    {
      auto generated = buildCapability(*child);
      if (!generated)
        return Result<QString>::Failed("Failed to generate capability: {}", generated.ErrorMessage());

      mAST.components.push_back(generated.Value());
    }

    auto taskAny = buildTask(*node, missionParameters);
    if (!taskAny)
      return Result<QString>::Failed("Failed to generate task: " + taskAny.ErrorMessage());

    mAST.components.push_back(taskAny.Value());
  }

  auto contents = KodaEmitter::emitKoda(mAST, mTypeRegistry);
  RETURN_ON_FAILURE_AS(contents, QString);

  return QString::fromStdString(contents.Value());
}

const maki::Value* MakiToKoda::getProperty(const QString& key, const INode& node) const
{
  for (const auto& property : node.getproperties())
    if (property->getid() == key)
      return dynamic_cast<const maki::Value*>(property->getvalue());

  return nullptr;
}

Result<koda::PComponent> MakiToKoda::buildTask(const INode& task, QVector<const IParameter*> missionParameters)
{
  auto c = std::make_shared<koda::Component>();
  c->kind = koda::Component::Kind::Task;
  if (const auto* prop = getProperty("name", task))
  {
    if (prop->toStringValue().toLower() == "task")
      RETURN_FAIL(task.getid(), Result<koda::PComponent>, "Tasks may not be called 'Task' or 'task'");

    c->name = format(prop->toStringValue().toLower(), "_");
  }
  else
  {
    RETURN_FAIL(task.getid(), Result<koda::PComponent>, "Task does not have a name");
  }

  // Get arguments
  for (const auto& cap : task.getchildren())
  {
    const auto* capName = getProperty("name", *cap);
    if (!capName)
      return Result<koda::PComponent>::Failed("Capability does not have a name");

    auto parg = std::make_shared<koda::Argument>();
    parg->kind = koda::Argument::Kind::Req;
    auto tmp = format(capName->toStringValue());
    ToLowerCase(tmp, 0);
    parg->a = types::TypeReference::named(capName->toStringValue().toStdString());
    parg->b = tmp;

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

  // Get mission parameters
  auto varsBlock = std::make_shared<koda::VarsBlock>();
  for (const auto* parameter : missionParameters)
  {
    auto generated = buildVarDef(parameter);
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
  const auto* name = getProperty("name", capability);
  if (!name)
    return Result<koda::PComponent>::Failed("Capability does not have a name");

  c->name = format(name->toStringValue());

  if (const auto* prop = getProperty("calldef", capability))
  {
    auto record = prop->toList();
    if (record.empty())
      return Result<koda::PComponent>::Failed("Call definition is empty: " + c->name);

    auto actions = buildActionDefs(capability, record);
    RETURN_ON_FAILURE_AS(actions, koda::PComponent);
    for (const auto& action : actions.Value())
    {
      auto statement = std::make_shared<koda::Statement>();
      statement->node = action;
      c->statements.push_back(statement);
    }
  }

  return c;
}

Result<koda::PVarDef> MakiToKoda::buildVarDef(const IParameter* property)
{
  if (!property)
    return Result<koda::PVarDef>::Failed("Cannot build mission parameter from null property");

  LOG_INFO("Generating var def for: {} {}", property->getname(), property->gettype().toString());
  auto varDef = std::make_shared<koda::VarDef>();
  varDef->name = property->getname().toStdString();
  varDef->varType = property->gettype();

  auto init = buildValueExpr(varDef->varType, maki::asValue(property->getvalue()));
  if (!init.IsSuccess())
    return Result<koda::PVarDef>::Failed("Could not build value for mission parameter '{}': {}", varDef->name, init.ErrorMessage());

  varDef->init = init.Value();

  return varDef;
}

Result<std::vector<koda::PActionDef>> MakiToKoda::buildActionDefs(const INode& node, const maki::ListValue& definition)
{
  std::vector<koda::PActionDef> actions;

  for (size_t i = 0; i < definition.size(); ++i)
  {
    if (definition.at(i).kind() != IValue::Kind::Record)
    {
      LOG_WARNING("Definition of {} in {} is not a record", definition.at(i).toReadable(), node.getnodeId());
      continue;
    }

    const auto item = definition.at(i).toRecord();
    if (!item.contains("type"))
      return Result<std::vector<koda::PActionDef>>::Failed("Definition missing type: {}", definition.at(i).toReadable());
    if (!item.contains("route"))
      return Result<std::vector<koda::PActionDef>>::Failed("Definition missing route: {}", definition.at(i).toReadable());
    if (!item.contains("message"))
      return Result<std::vector<koda::PActionDef>>::Failed("Definition missing message: {}", definition.at(i).toReadable());

    auto callType = item.at("type").toString();
    auto callRoute = item.at("route").toString();
    auto callMessage = item.at("message").toString();

    auto action = std::make_shared<koda::ActionDef>();
    if (callType.trimmed() == "action")
      action->kind = koda::ActionDef::Kind::Action;
    else if (callType.trimmed() == "service")
      action->kind = koda::ActionDef::Kind::Service;
    else if (callType.trimmed() == "topic")
      action->kind = koda::ActionDef::Kind::Topic;
    else
      return Result<std::vector<koda::PActionDef>>::Failed("Unknown action definition type: {}", callType.trimmed().toStdString());

    action->label1 = callRoute.toStdString();
    action->label2 = callMessage.toStdString();
    for (const auto& event : node.getevents())
    {
      if (event->getlinksTo() != static_cast<int>(i))
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
    return Result<koda::PRosDef>::Failed("Unknown call type: {} in {}", (int)event.gettype(), event.getname());

  auto eventDef = std::make_shared<koda::EventDef>();
  eventDef->typeName = event.getreturnType().toString();
  eventDef->name = event.getname().toStdString();

  for (const auto& arg : event.getarguments())
  {
    auto parg = std::make_shared<koda::Argument>();
    parg->kind = koda::Argument::Kind::Plain;
    parg->a = arg->gettype();
    parg->b = arg->getid().toStdString();
    eventDef->args.push_back(parg);
  }

  def->def = eventDef;

  return def;
}

Result<koda::PFlow> MakiToKoda::buildFlowAst(const IFlow& flow)
{
  auto flowName = format(flow.getname());
  const auto* start = findStartNode(flow);
  if (start == nullptr)
    return Result<koda::PFlow>::Failed("Flow has no Koda::Start node");

  auto seq = buildSequenceFrom(flow, start, nullptr);
  if (!seq.has_value())
    return Result<koda::PFlow>::Failed("Failed to build first sequence");

  auto pflow = std::make_shared<koda::Flow>();
  if (flowName != "main")
    flowName = "f" + flowName;

  for (const auto& argument : flow.getarguments())
  {
    auto arg = std::make_shared<koda::Argument>();
    arg->kind = koda::Argument::Kind::Plain;
    arg->a = argument->gettype();
    arg->b = argument->getid().toStdString();
    pflow->args.push_back(arg);
  }
  pflow->id = flow.getid().toStdString();
  pflow->name = flowName;
  pflow->strategy = std::any_cast<koda::PStrategy>(seq);
  if (mTraceMap)
    mTraceMap->mapAst(pflow->id, MakiSource{
                                     .id = pflow->id,
                                     .flowId = pflow->id,
                                     .kind = MakiElementKind::Flow,
                                 });

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
  std::any result;

  if (node.getnodeId() == "Koda::Async task")
    result = buildAsyncExpr(flow, node);
  else if (node.getnodeId() == "Koda::Sync task")
    result = buildSyncExpr(flow, node);
  else if (node.getnodeId() == "Koda::Flow call")
    result = buildStrategyExpr(flow, node);
  else if (node.getnodeId() == "Koda::Within")
    result = buildWithinExpr(flow, node);
  else if (node.getnodeId() == "Koda::Repeat")
    result = buildRepeatExpr(flow, node);
  else if (node.getnodeId() == "Koda::Continue")
    result = buildContinueExpr(flow, node);
  else if (node.getnodeId() == "Koda::Terminate")
    result = buildSuccessExpr(flow, node);
  else
  {
    LOG_ERROR("Unknown expression: {}", node.getnodeId());
    return result;
  }

  auto strategy = std::any_cast<koda::PStrategy>(result);
  strategy->id = std::format("{}_{}", node.getid(), flow.getid());
  traceNode(flow, node, strategy);

  return result;
}

void MakiToKoda::traceNode(const IFlow& flow, const INode& node, const koda::PStrategy& strategy)
{
  if (!mTraceMap)
    return;

  MakiElementKind kind = MakiElementKind::Node;
  if (node.getnodeId() == "Koda::Async task")
    kind = MakiElementKind::Async;
  else if (node.getnodeId() == "Koda::Sync task")
    kind = MakiElementKind::Sync;

  mTraceMap->mapAst(strategy->id, MakiSource{
                                      .id = node.getid().toStdString(),
                                      .flowId = flow.getid().toStdString(),
                                      .kind = kind,
                                  });
}

std::any MakiToKoda::buildAsyncExpr(const IFlow& flow, const INode& node)
{
  const auto* cap = getProperty("capability", node);
  if (!cap)
    LOG_AND_FAIL(node.getid(), flow.getid(), "AsyncTask component does not have a valid capability");
  if (!cap->isRecord())
    LOG_AND_FAIL(node.getid(), flow.getid(), "AsyncTask component capability does not have a valid format: {}", cap->toReadable());

  const auto record = cap->toRecord();
  if (!record.contains("component"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component name missing in async call: {}", cap->toReadable());
  if (!record.at("component").isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component name has wrong format in async call: {}", cap->toReadable());
  if (!record.contains("arguments"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component arguments missing in async call: {}", cap->toReadable());
  if (!record.at("arguments").isList())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component arguments has wrong format in async call: {}", cap->toReadable());

  auto call = std::make_shared<koda::EventCall>();
  call->name = format(maki::recordString(record, "component"));
  ToLowerCase(call->name, 0);
  call->args = buildArgumentExpr(maki::recordList(record, "arguments"));

  auto expr = std::make_shared<koda::Strategy::TaskCall>();
  expr->call = call;

  auto handlers = buildHandlers(flow, node);
  for (const auto& handler : handlers)
    expr->handlers.push_back(handler);

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;
  return strat;
}

std::any MakiToKoda::buildSyncExpr(const IFlow& flow, const INode& node)
{
  const auto* cap = getProperty("capability", node);
  if (!cap)
    LOG_AND_FAIL(node.getid(), flow.getid(), "SyncTask component does not have a valid capability");
  if (!cap->isRecord())
    LOG_AND_FAIL(node.getid(), flow.getid(), "SyncTask component capability does not have a valid format: {}", cap->toReadable());

  const auto record = cap->toRecord();
  if (!record.contains("component"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component name missing in sync call: {}", cap->toReadable());
  if (!record.at("component").isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component name has wrong format in sync call: {}", cap->toReadable());

  if (!record.contains("event"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component event missing in sync call: {}", cap->toReadable());
  if (!record.at("event").isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component event has wrong format in sync call: {}", cap->toReadable());

  if (!record.contains("arguments"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component arguments missing in sync call: {}", cap->toReadable());
  if (!record.at("arguments").isList())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Component arguments has wrong format in sync call: {}", cap->toReadable());

  auto call = std::make_shared<koda::EventCall>();
  call->receiver = format(maki::recordString(record, "component"));
  ToLowerCase(call->receiver, 0);
  call->name = maki::recordString(record, "event").toStdString();
  call->args = buildArgumentExpr(maki::recordList(record, "arguments"));

  auto expr = std::make_shared<koda::Strategy::TaskCall>();
  expr->call = call;

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;
  return strat;
}

std::any MakiToKoda::buildStrategyExpr(const IFlow& flow, const INode& node)
{
  const auto* cap = getProperty("task", node);
  if (!cap)
    LOG_AND_FAIL(node.getid(), flow.getid(), "Flow call does not have a valid task");
  if (!cap->isRecord())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Flow call task does not have a valid format: {}", cap->toReadable());

  const auto record = cap->toRecord();
  if (!record.contains("flow"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Flow name missing in flow call: {}", cap->toReadable());
  if (!record.at("flow").isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Flow name has wrong format in flow call: {}", cap->toReadable());

  if (!record.contains("arguments"))
    LOG_AND_FAIL(node.getid(), flow.getid(), "Flow arguments missing in flow call: {}", cap->toReadable());
  if (!record.at("arguments").isList())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Flow arguments has wrong format in flow call: {}", cap->toReadable());

  auto call = std::make_shared<koda::EventCall>();
  call->name = "f" + format(maki::recordString(record, "flow"));
  call->args = buildArgumentExpr(maki::recordList(record, "arguments"));

  auto expr = std::make_shared<koda::Strategy::TaskCall>();
  expr->call = call;

  auto handlers = buildHandlers(flow, node);
  for (const auto& handler : handlers)
    expr->handlers.push_back(handler);

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

  auto timeout = getProperty("timeout", node);
  if (!timeout)
    LOG_AND_FAIL(node.getid(), flow.getid(), "Within missing timeout property");
  if (!timeout->isInt() && !timeout->isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Within timeout property should be an integer");

  expr->seconds = timeout->toInt();

  auto handlers = buildHandlers(flow, node);
  for (const auto& handler : handlers)
    expr->handlers.push_back(handler);

  auto strat = std::make_shared<koda::Strategy>();
  strat->v = expr;

  return strat;
}

std::any MakiToKoda::buildRepeatExpr(const IFlow& flow, const INode& node)
{
  const auto* task = getProperty("task", node);
  if (!task)
    LOG_AND_FAIL(node.getid(), flow.getid(), "Repeat task property is missing");
  if (!task->isRecord())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Repeat task property should be a record");

  const auto* iterations = getProperty("iterations", node);
  if (!iterations)
    LOG_AND_FAIL(node.getid(), flow.getid(), "Repeat iterations property is missing");
  if (!iterations->isInt() && !iterations->isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Repeat iterations property should be an integer");

  const auto* rate = getProperty("rate", node);
  if (!rate)
    LOG_AND_FAIL(node.getid(), flow.getid(), "Repeat rate property is missing");
  if (!rate->isInt() && !rate->isString())
    LOG_AND_FAIL(node.getid(), flow.getid(), "Repeat rate property should be an integer");

  auto record = task->toRecord();
  auto call = std::make_shared<koda::EventCall>();
  call->name = "f" + format(maki::recordString(record, "flow"));
  call->args = buildArgumentExpr(maki::recordList(record, "arguments"));

  auto flowCall = std::make_shared<koda::Strategy::TaskCall>();
  flowCall->call = call;

  auto expr = std::make_shared<koda::Strategy::Repeat>();
  expr->iterations = iterations->toInt();
  expr->seconds = rate->toInt();

  auto handlers = buildHandlers(flow, node);
  for (const auto& handler : handlers)
    expr->handlers.push_back(handler);

  auto repeatStrat = std::make_shared<koda::Strategy>();
  repeatStrat->v = flowCall;
  expr->a = repeatStrat;

  const auto seqSuccessors = sequentialSuccessorsOf(node, flow);
  if (!seqSuccessors.empty())
  {
    auto sequence = buildSequenceFrom(flow, seqSuccessors.first().node, nullptr);
    if (!sequence.has_value())
      LOG_AND_FAIL(node.getid(), flow.getid(), "Failed to create do sequence");
  }

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
    auto event = signalStart.transition->getevent().toStdString();
    auto receiverIndex = event.find_first_of('.');
    emitter->receiver = event.substr(receiverIndex + 1);
    emitter->name = event.substr(0, receiverIndex);
    ToLowerCase(emitter->name, 0);

    value->emitter = emitter;

    handlers.append(value);
  }

  return handlers;
}

std::shared_ptr<koda::Expr> MakiToKoda::buildExpr(const maki::Value& value)
{
  auto wrapper = std::make_shared<koda::Expr>();
  switch (value.kind())
  {
    case IValue::Kind::Bool:
    {
      auto expr = std::make_shared<koda::Expr::Bool>();
      expr->value = value.toBool();
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::Int:
    {
      auto expr = std::make_shared<koda::Expr::Int>();
      expr->value = value.toInt();
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::Double:
    {
      auto expr = std::make_shared<koda::Expr::Float>();
      expr->value = value.toDouble();
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::StdString:
    case IValue::Kind::QString:
    {
      auto expr = std::make_shared<koda::Expr::Str>();
      expr->value = value.toString().toStdString();
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::List:
    {
      auto expr = std::make_shared<koda::Expr::ListLiteral>();
      for (const auto& item : value.toList())
        expr->fields.push_back(buildExpr(item));
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::Map:
    {
      auto expr = std::make_shared<koda::Expr::MapLiteral>();
      for (const auto& [key, item] : value.toMap())
      {
        auto field = std::make_shared<koda::Expr::MapLiteral::Field>();
        field->key = buildExpr(key);
        field->value = buildExpr(item);
        expr->fields.push_back(field);
      }
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::Record:
    {
      auto expr = std::make_shared<koda::Expr::RecordLiteral>();
      for (const auto& [key, item] : value.toRecord())
      {
        auto field = std::make_shared<koda::Expr::RecordLiteral::Field>();
        field->name = key;
        field->value = buildExpr(item);
        expr->fields.push_back(field);
      }
      wrapper->v = expr;
      break;
    }
    case IValue::Kind::Color:
    default:
    {
      LOG_ERROR("Value not supported in expression: {}", value.toReadable());
      return nullptr;
    }
  }

  return wrapper;
}

std::vector<std::shared_ptr<Expr>> MakiToKoda::buildArgumentExpr(const maki::ListValue& list)
{
  std::vector<std::shared_ptr<Expr>> args = {};

  for (const auto& arg : list)
  {
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
    if (node->getnodeId() == "Koda::Start")
      return node.get();

  return nullptr;
}

const INode* MakiToKoda::findDestination(const QString& dstId, const IFlow& flow) const
{
  for (const auto& node : flow.getnodes())
    if (node->getid() == dstId)
      return node.get();

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
  return node.getnodeId() == "Koda::Start" || node.getnodeId() == "Koda::Success" || node.getnodeId() == "Koda::Failure" ||
         node.getnodeId() == "Koda::Join";
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

Result<koda::PExpr> MakiToKoda::buildValueExpr(const koda::types::TypeReference& type, const maki::Value* value)
{
  auto wrapper = std::make_shared<koda::Expr>();
  if (type.isPrimitive())
  {
    switch (type.primitiveKind())
    {
      case koda::types::PrimitiveKind::Bool:
      {
        auto expr = std::make_shared<koda::Expr::Bool>();
        expr->value = value->toBoolValue();
        wrapper->v = expr;
        return wrapper;
      }

      case koda::types::PrimitiveKind::Int8:
      case koda::types::PrimitiveKind::Int16:
      case koda::types::PrimitiveKind::Int32:
      case koda::types::PrimitiveKind::Int64:
      case koda::types::PrimitiveKind::UInt8:
      case koda::types::PrimitiveKind::UInt16:
      case koda::types::PrimitiveKind::UInt32:
      case koda::types::PrimitiveKind::UInt64:
      {
        auto expr = std::make_shared<koda::Expr::Int>();
        expr->value = value->toIntValue();
        wrapper->v = expr;
        return wrapper;
      }

      case koda::types::PrimitiveKind::Float32:
      case koda::types::PrimitiveKind::Float64:
      {
        auto expr = std::make_shared<koda::Expr::Float>();
        expr->value = value->toDoubleValue();
        wrapper->v = expr;
        return wrapper;
      }

      case koda::types::PrimitiveKind::String:
      {
        auto expr = std::make_shared<koda::Expr::Str>();
        expr->value = value->toStringValue().toStdString();
        wrapper->v = expr;
        return wrapper;
      }

      default:
        return Result<koda::PExpr>::Failed("Unsupported primitive parameter type '{}'", type.toString());
    }
  }
  else if (type.isList())
  {
    if (!value->isList())
      return Result<koda::PExpr>::Failed("Expected list value for type '{}'", type.toString());

    auto literal = std::make_shared<koda::Expr::ListLiteral>();

    const auto values = value->toListValue();
    const auto elementType = type.elementType();

    for (const auto& item : values)
    {
      auto built = buildValueExpr(elementType, maki::asValue(item.get()));
      if (!built.IsSuccess())
        return Result<koda::PExpr>::Failed("Could not build list element for '{}': {}", type.toString(), built.ErrorMessage());

      literal->fields.push_back(built.Value());
    }

    wrapper->v = literal;
    return wrapper;
  }
  else if (type.isMap())
  {
    if (value->kind() != IValue::Kind::Map)
      return Result<koda::PExpr>::Failed("Expected map value for type '{}'", type.toString());

    auto literal = std::make_shared<koda::Expr::MapLiteral>();

    const auto keyType = type.mapKeyType();
    const auto valueType = type.mapValueType();
    for (const auto& [key, val] : value->toMapValue())
    {
      auto builtKey = buildValueExpr(keyType, maki::asValue(key.get()));
      if (!builtKey.IsSuccess())
        return Result<koda::PExpr>::Failed("Could not build map key for '{}': {}", type.toString(), builtKey.ErrorMessage());

      auto builtValue = buildValueExpr(valueType, maki::asValue(val.get()));
      if (!builtValue.IsSuccess())
        return Result<koda::PExpr>::Failed("Could not build map value for '{}': {}", type.toString(), builtValue.ErrorMessage());

      auto fieldLiteral = std::make_shared<koda::Expr::MapLiteral::Field>();
      fieldLiteral->key = builtKey.Value();
      fieldLiteral->value = builtValue.Value();
      literal->fields.push_back(fieldLiteral);
    }

    wrapper->v = literal;
    return wrapper;
  }
  else if (type.isNamed())
  {
    const auto* definition = mTypeRegistry->resolve(type);
    if (!definition)
      return Result<koda::PExpr>::Failed("Could not resolve type '{}'", type.toString());

    if (definition->isAlias())
    {
      return buildValueExpr(definition->alias().target, value);
    }
    else if (definition->isRecord())
    {
      if (value->kind() != IValue::Kind::Record)
        return Result<koda::PExpr>::Failed("Expected record value for type '{}'", type.toString());

      const auto values = value->toRecordValue();
      const auto& record = definition->record();

      auto literal = std::make_shared<koda::Expr::RecordLiteral>();
      for (const auto& field : record.fields)
      {
        const auto valueIt = values.find(field.name);

        // Allows partial/default initialization, including {}.
        if (valueIt == values.end())
          continue;

        auto built = buildValueExpr(field.type, maki::asValue(valueIt->second.get()));
        if (!built.IsSuccess())
          return Result<koda::PExpr>::Failed("Could not build field '{}.{}': {}", type.toString(), field.name, built.ErrorMessage());

        auto fieldLiteral = std::make_shared<koda::Expr::RecordLiteral::Field>();
        fieldLiteral->name = field.name;
        fieldLiteral->value = built.Value();
        literal->fields.push_back(fieldLiteral);
      }

      wrapper->v = literal;
      return wrapper;
    }
    else if (definition->isEnum())
    {
      if (value->kind() != IValue::Kind::StdString && value->kind() != IValue::Kind::QString)
        return Result<koda::PExpr>::Failed("Expected string-like value for enum '{}'", type.toString());

      auto expr = std::make_shared<koda::Expr::Id>();
      expr->value = value->toStringValue().toStdString();

      wrapper->v = expr;
      return wrapper;
    }

    return Result<koda::PExpr>::Failed("Unsupported named parameter type '{}'", type.toString());
  }

  return Result<koda::PExpr>::Failed("Unsupported parameter type '{}'", type.toString());
}
}  // namespace koda
