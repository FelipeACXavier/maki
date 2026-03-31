#pragma once

#include "canvas.h"

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
public:
  /**
   * @brief Constructs a BehaviourCanvas object.
   *
   * @param flow A pointer to the Flow object associated with this canvas.
   * @param storage A shared pointer to the SaveInfo object for managing storage.
   * @param configTable A shared pointer to the ConfigurationTable for configuration data.
   * @param parent An optional QObject parent for this canvas.
   */
  BehaviourCanvas(Flow* flow, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  /**
   * @brief Retrieves the type of library associated with this canvas.
   *
   * @return The library type as a value of `Types::LibraryTypes`.
   */
  Types::LibraryTypes type() const override;

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
};
