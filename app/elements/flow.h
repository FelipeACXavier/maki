#pragma once

#include <QMap>
#include <QString>

#include "save_info.h"
#include "types.h"

class NodeItem;
class ConnectionItem;

class Flow
{
public:
  enum
  {
    Type = Types::FLOW
  };

  Flow(const QString& flowName);

  QString id() const;
  int type() const;
  QString name() const;

  // Contains a list of saved data of the behaviour nodes
  // Add, remove, and edit nodes
  void addNode(NodeItem* node);
  void removeNode(NodeItem* node);

  void updateFlow(NodeItem* node);

  void addConnection(ConnectionItem* connection);
  void removeConnection(ConnectionItem* connection);

  QVector<NodeSaveInfo> getNodes() const;
  QVector<ConnectionSaveInfo> getConnections() const;

private:
  const QString mId;
  QString mName;

  SaveInfo mInfo;
};
