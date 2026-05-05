#pragma once

#include <QGraphicsView>

#include "config.h"
#include "result.h"

/**
 * @brief A custom QGraphicsView for managing a library of nodes.
 */
class LibraryContainer : public QGraphicsView
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new LibraryContainer with the given parent widget.
   *
   * @param parent The parent widget.
   */
  LibraryContainer(QWidget* parent = nullptr);

  /**
   * @brief Creates and returns a new LibraryContainer instance with the specified name and parent widget.
   *
   * @param name The name of the library container.
   * @param parent The parent widget.
   * @return A pointer to the newly created LibraryContainer instance.
   */
  static LibraryContainer* create(const QString& name, QWidget* parent);

  /**
   * @brief Adds a node with the given ID and configuration to the library.
   *
   * @param id The unique identifier for the node.
   * @param config The configuration for the node.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult addNode(const QString& id, std::shared_ptr<NodeConfig> config);

  /**
   * @brief Filters nodes based on the given query string.
   *
   * @param query The query string used to filter nodes.
   * @return true if any nodes were filtered; false otherwise.
   */
  bool filterNodes(const QString& query);

protected:
  /**
   * @brief Handles resize events for the LibraryContainer.
   *
   * @param event The resize event.
   */
  void resizeEvent(QResizeEvent* event) override;

private:
  int mLastItemY;  /// The last Y position of an item in the scene.

  /**
   * @brief Updates the size of the scene based on the current view size.
   */
  void updateSceneSize();

  /**
   * @brief Adjusts the positions of nodes within the scene.
   */
  void adjustNodePositions();
};
