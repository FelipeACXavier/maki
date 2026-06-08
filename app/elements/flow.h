#pragma once

#include <QMap>
#include <QString>

#include "elements/transition.h"
#include "ids.h"
#include "save_info.h"

class NodeItem;
class TransitionItem;

/**
 * @brief Represents a flow in the system.
 */
class Flow
{
public:
  enum
  {
    Type = Types::FLOW
  };

  /**
   * @brief Constructs a new Flow object.
   *
   * @param flowName The name of the flow.
   * @param storage A shared pointer to the flow save information.
   */
  Flow(const QString& flowName, std::shared_ptr<FlowSaveInfo> storage);

  /**
   * @brief Returns the unique identifier of the flow.
   *
   * @return const QString& The flow's ID.
   */
  QString id() const;

  /**
   * @brief Returns the type of the flow.
   *
   * @return int The flow's type.
   */
  int type() const;

  /**
   * @brief Returns the name of the flow.
   *
   * @return QString The flow's name.
   */
  QString name() const;

  /**
   * @brief Checks if the flow is modifiable.
   *
   * @return bool True if modifiable, false otherwise.
   */
  bool modifiable() const;

  /**
   * @brief Removes a node from the flow.
   *
   * @param node The node to remove.
   */
  void removeNode(NodeItem* node);

  /**
   * @brief Updates a node in the flow with new save information.
   *
   * @param node The node to update.
   * @param storage A shared pointer to the new node save information.
   */
  void updateFlow(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage);

  /**
   * @brief Retrieves a list of all nodes in the flow.
   *
   * @return QVector<std::shared_ptr<NodeSaveInfo>> The list of nodes.
   */
  QVector<std::shared_ptr<NodeSaveInfo>> getNodes() const;

  /**
   * @brief Returns a list of transitions connected to this node.
   *
   * @return The QVector of TransitionItem pointers.
   */
  QVector<TransitionItem*> transitions() const;

  /**
   * @brief Adds a transition to the node.
   *
   * @param transition The TransitionItem to add.
   */
  void addTransition(TransitionItem* transition);

  /**
   * @brief Removes a transition from the node.
   *
   * @param transition The TransitionItem to remove.
   */
  void removeTransition(TransitionItem* transition);
  void deleteTransition(TransitionItem* transition);

  std::shared_ptr<FlowSaveInfo> config() const;

private:
  /// Unique identifier for the flow
  const QString mId;

  /// Name of the flow
  QString mName;

  /// Storage for flow save information
  std::shared_ptr<FlowSaveInfo> mStorage;

  /// List of transitions in this flow
  QVector<TransitionItem*> mTransitions;
};
