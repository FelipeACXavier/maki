#pragma once

#include "canvas.h"
#include "elements/transition.h"

/**
 * @class BehaviourCanvas
 * @brief A specialized canvas for managing and displaying behavior-related nodes.
 *
 * This class extends the `Canvas` class and provides functionality specific to
 * handling behavior-related nodes in the system. It interacts with a single `Flow` object
 * and utilizes storage and configuration data.
 */
class BehaviourCanvas : public Canvas
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a BehaviourCanvas object.
   *
   * @param flow A pointer to the Flow object associated with this canvas.
   * @param storage A shared pointer to the SaveInfo object for managing storage.
   * @param configTable A shared pointer to the ConfigurationTable for configuration data.
   * @param parent An optional QObject parent for this canvas.
   */
  BehaviourCanvas(Flow* flow, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  /**
   * @brief Retrieves the type of library associated with this canvas.
   *
   * @return The library type as a value of `Types::LibraryTypes`.
   */
  virtual Types::LibraryTypes type() const override;

  /**
   * @brief Checkes whether it is possible to add a new trasition to a node
   * @param node A pointer to the node of interest
   * @return True if the node allows more transitions, and false otherwise.
   */
  bool canAddTransition(NodeItem* node) const override;

  /**
   * @brief Gets the next transition configuration for a given node
   *
   * This method assumes the caller already called canAddTransition
   * @param node A pointer to the node of interest
   *
   * @return The TransitionConfig object of the next allowed transition
   */
  TransitionConfig nextTransition(NodeItem* node) const override;

  /**
   * @brief Remove all transitions to and from a node with id nodeId
   */
  QVector<QGraphicsItem*> cleanTransitionsOfNode(const QString& nodeId) override;

  /**
   * @brief Add the TransitionItem to the Flow managed by this Canvas
   */
  void addTransition(TransitionItem* transition) override;

  /**
   * @brief Removed the TransitionItem from the Flow managed by this Canvas
   */
  void removeTransition(TransitionItem* transition) override;

  /**
   * @brief Callback triggered whenever a node moves significantly
   */
  void onNodeMoved(const QString& nodeId) override;

protected:
  /**
   * @brief Deletes the Flow managed by this Canvas. This should not be called if the Flow is created by a NodeItem.
   */
  void cleanFlow();

private:
  /**
   * @brief A pointer to the Flow object associated with this canvas.
   */
  Flow* mFlow;

  /**
   * @brief Updates the parent node of a given node item.
   *
   * This method is used to update the parent-child relationship of nodes in the canvas.
   *
   * @param node A pointer to the NodeItem whose parent is being updated.
   * @param storage A shared pointer to the NodeSaveInfo object for managing node storage.
   * @param adding A boolean indicating whether the node is being added or removed.
   */
  void updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding) override;

  void addInitialNode(const QString& nodeType, const QPointF& position);
};
