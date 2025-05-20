#pragma once

#include <QMap>
#include <QString>

#include "save_info.h"
#include "types.h"

class NodeItem;

class Flow
{
public:
  enum
  {
    Type = Types::FLOW
  };

  Flow(const QString& flowName, std::shared_ptr<FlowSaveInfo> storage);

  QString id() const;
  int type() const;
  QString name() const;

  // Contains a list of saved data of the behaviour nodes
  // Add, remove, and edit nodes
  void removeNode(NodeItem* node);

  void updateFlow(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage);

  QVector<std::shared_ptr<NodeSaveInfo>> getNodes() const;

private:
  const QString mId;
  QString mName;

  std::shared_ptr<FlowSaveInfo> mStorage;
};
