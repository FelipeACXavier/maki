#include "dezyne_simulator.h"

#include <QJsonObject>

#include "dezyne_client.h"
#include "logging.h"

DezyneSimulator::DezyneSimulator(QObject* parent)
    : QObject(parent)
    , mState(State::Idle)
    , mModel("")
    , mWorkDir("")
    , mSimulationId("sim-123")
    , mIncludes({})
{
  mClient = new DezyneClient(parent);
  QObject::connect(mClient, &DezyneClient::connected, this, &DezyneSimulator::onConnected);
  QObject::connect(mClient, &DezyneClient::disconnected, this, &DezyneSimulator::onDisconnected);
  QObject::connect(mClient, &DezyneClient::errorOccurred, this, &DezyneSimulator::onError);
  QObject::connect(mClient, &DezyneClient::messageReceivedJson, this, &DezyneSimulator::onMessageReceived);
}

VoidResult DezyneSimulator::startSimulation(const QString& id)
{
  if (!mClient)
    return VoidResult::Failed("No client available");
  else if (mModel.isEmpty())
    return VoidResult::Failed("No model defined");
  else if (mWorkDir.isEmpty())
    return VoidResult::Failed("No working directory defined");

  LOG_DEBUG("Connecting to daemon");

  mState = State::Starting;

  // TODO(felaze): Update once the daemon is fixed
  // mSimulationId = id;

  mClient->setUrl(QUrl("ws://localhost:3001"));
  mClient->connectToServer();

  return VoidResult();
}

void DezyneSimulator::stopSimulation()
{
  mState = State::Stopping;
  mClient->disconnectFromServer();
}

void DezyneSimulator::triggerEvent(const QString& eventName)
{
  QJsonObject object;
  object["type"] = "SEQUENCE_DIAGRAM_EVENT";
  object["simulationId"] = mSimulationId;
  object["event"] = eventName;

  mClient->sendJson(object);
}

void DezyneSimulator::setSimulationModel(const QString& model)
{
  mModel = model;
}

void DezyneSimulator::setWorkingDirectory(const QString& workingDirectory)
{
  mWorkDir = workingDirectory;
}

void DezyneSimulator::setSimulationIncludes(const QList<QString>& path)
{
  mIncludes = path;
}

void DezyneSimulator::onConnected()
{
  LOG_DEBUG("Connected to Dezyne daemon");
  if (mModel.isEmpty())
  {
    LOG_DEBUG("No model, not starting the simulation");
    return;
  }

  QJsonObject object;
  object["id"] = QUuid::createUuid().toString();
  object["type"] = "START_SIMULATION_COMMAND";
  object["name"] = "START_SIMULATION_COMMAND";
  object["timestamp"] = QString::fromStdString(logging::TimeToString(std::chrono::system_clock::now()));
  object["workingDirectory"] = mWorkDir;

  QJsonObject args;
  args["threads"] = "--threads=1";
  object["rootArgs"] = args;

  object["filePath"] = mModel;

  QJsonArray importPath;
  for (const auto& path : mIncludes)
    importPath.append(path);
  // importPath.append("/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo/../");
  // importPath.append("/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo/../examples");

  object["importPath"] = importPath;

  mClient->sendJson(object);
}

void DezyneSimulator::onDisconnected()
{
  LOG_DEBUG("Disconnected from Dezyne daemon");
  mState = State::Idle;
}

void DezyneSimulator::onError(const QString& error)
{
  LOG_ERROR("Error in daemon: {}", qPrintable(error));
  mState = State::Idle;
}

void DezyneSimulator::onMessageReceived(const QJsonObject& obj)
{
  LOG_DEBUG("Message received");

  // auto pretty = QJsonDocument(obj).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Received message: {}", qPrintable(pretty));

  if (!obj.contains("type"))
  {
    LOG_WARNING("Message with no type");
    return;
  }

  auto type = obj["type"].toString();
  if (type == "SIMULATION_STATUS_RESPONSE")
    handleSimulationStarted(obj);
  else if (type == "TRACE_UPDATE")
    handleSimulationUpdate(obj);
}

void DezyneSimulator::handleSimulationStarted(const QJsonObject& obj)
{
  if (!obj.contains("success"))
  {
    LOG_WARNING("Simulation response does not contain \"success\" field");
    return;
  }

  auto success = obj["success"].toBool();
  if (!success)
  {
    mState = State::Idle;
    LOG_WARNING("Failed to start simulation");
  }

  mState = State::Running;
  QJsonObject object;
  object["type"] = "REGISTER_SEQUENCE_DIAGRAM_VIEWER";
  object["simulationId"] = mSimulationId;
  mClient->sendJson(object);

  emit simulationStarted();
}

void DezyneSimulator::handleSimulationUpdate(const QJsonObject& obj)
{
  if (!obj.contains("traceData"))
  {
    LOG_WARNING("Simulation response does not contain \"traceData\" field");
    return;
  }

  auto data = obj["traceData"].toObject();
  emit simulationUpdated(data);
}
