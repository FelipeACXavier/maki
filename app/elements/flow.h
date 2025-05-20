#pragma once

#include <QMap>
#include <QString>

#include "types.h"

class NodeSaveInfo;
class NodeItem;

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
  void updateFlow(NodeItem* node);
  void getNodes();

private:
  const QString mId;
  QString mName;

  QMap<QString, NodeSaveInfo> mNodes;
};
