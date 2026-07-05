#pragma once

#include "elements/node.h"

class NodeConfig;
class NodeSaveInfo;

/**
 * @brief Registry-based factory for creating the appropriate NodeItem subclass.
 */
class NodeFactory
{
public:
  using Creator = NodeItem* (*)(const QString& id,
                                  std::shared_ptr<NodeSaveInfo> info,
                                  const QPointF& position,
                                  std::shared_ptr<NodeConfig> config,
                                  QGraphicsItem* parent);

  static void registerType(const QString& typeKey, Creator creator);
  static NodeItem* create(const QString& id,
                          std::shared_ptr<NodeSaveInfo> info,
                          const QPointF& position,
                          std::shared_ptr<NodeConfig> config,
                          QGraphicsItem* parent = nullptr);

  /** Registers all built-in Koda behavioural node types. Idempotent. */
  static void registerBuiltins();

private:
  static Creator lookup(const QString& typeKey);
};
