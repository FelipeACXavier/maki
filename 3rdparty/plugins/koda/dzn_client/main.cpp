#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

#include "dezyne_simulator.h"

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  auto simulator = std::make_shared<DezyneSimulator>(nullptr);

  simulator->setWorkingDirectory("/home/felaze/Documents/PhD/Programs/behaviour_tree/monorepo");
  simulator->setSimulationModel("/home/felaze/Documents/PhD/Programs/behaviour_tree/examples/task.dzn");
  QList<QString> includes = {
      "/home/felaze/Documents/PhD/Programs/behaviour_tree"
      "/home/felaze/Documents/PhD/Programs/behaviour_tree/examples"};
  simulator->setSimulationIncludes(includes);

  simulator->startSimulation("sim-12345");

  return app.exec();
}
