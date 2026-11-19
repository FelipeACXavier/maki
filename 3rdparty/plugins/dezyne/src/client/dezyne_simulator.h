#pragma once

#include <QObject>

#include "result.h"

class DezyneClient;

class DezyneSimulator : public QObject
{
  Q_OBJECT
public:
  DezyneSimulator(QObject* parent = nullptr);

  VoidResult startSimulation(const QString& id);
  void stopSimulation();

  void setSimulationModel(const QString& model);
  void setWorkingDirectory(const QString& workingDirectory);
  void setSimulationIncludes(const QList<QString>& path);

  void triggerEvent(const QString& eventName);

signals:
  void simulationStarted();
  void simulationUpdated(const QJsonObject& obj);

private slots:
  void onConnected();
  void onDisconnected();
  void onError(const QString& error);
  void onMessageReceived(const QJsonObject& obj);

private:
  enum class State
  {
    Unknown = 0,
    Idle,
    Starting,
    Running,
    Stopping
  } mState;

  DezyneClient* mClient;
  QString mModel;
  QString mWorkDir;
  QString mSimulationId;
  QList<QString> mIncludes;

  void handleSimulationStarted(const QJsonObject& obj);
  void handleSimulationUpdate(const QJsonObject& obj);
};