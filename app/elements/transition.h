#pragma once

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>

#include "ids.h"

class NodeItem;
class TransitionSaveInfo;

/**
 * @brief Represents a transition between two nodes in a graphical representation.
 */
class TransitionItem : public QGraphicsPathItem
{
public:
  enum
  {
    Type = Types::TRANSITION
  };

  /**
   * @brief Enumerates the types of edges that can be used for transitions.
   */
  enum class Edge
  {
    NONE = 0,  /// No edge type.
    FORWARD,   /// Forward transition edge.
    BACKWARD,  /// Backward transition edge.
    ARC        /// Arc-shaped transition edge.
  };

  /**
   * @brief Constructs a new TransitionItem with the given storage information.
   * @param storage Shared pointer to the transition save info.
   */
  TransitionItem(std::shared_ptr<TransitionSaveInfo> storage);

  /**
   * @brief Destructor for TransitionItem.
   */
  virtual ~TransitionItem();

  /**
   * @brief Retrieves the unique identifier of this transition item.
   * @return The ID as a QString.
   */
  QString id() const;

  /**
   * @brief Retrieves the type of this graphics item.
   * @return The type as an integer.
   */
  int type() const override;

  /**
   * @brief Sets the start point and control shift for the transition.
   * @param id Unique identifier for the start node.
   * @param point Start position.
   * @param controlShift Control point shift.
   */
  void setStart(const QString& id, const QPointF& point, const QPointF& controlShift);

  /**
   * @brief Sets the end point and control shift for the transition.
   * @param id Unique identifier for the end node.
   * @param point End position.
   * @param controlShift Control point shift.
   */
  void setEnd(const QString& id, const QPointF& point, const QPointF& controlShift);

  /**
   * @brief Completes the transition by setting source and destination nodes.
   * @param source Pointer to the source node item.
   * @param destination Pointer to the destination node item.
   */
  void done(NodeItem* source, NodeItem* destination);

  /**
   * @brief Retrieves the source node of this transition.
   * @return Pointer to the source node item.
   */
  NodeItem* source() const;

  /**
   * @brief Retrieves the destination node of this transition.
   * @return Pointer to the destination node item.
   */
  NodeItem* destination() const;

  /**
   * @brief Moves the transition to a new position.
   * @param id Unique identifier for the node being moved.
   * @param pos New position.
   */
  void move(const QString& id, QPointF pos);

  /**
   * @brief Updates the path of this transition item.
   */
  void updatePath(QPainterPath painterPath = {});

  /**
   * @brief Paints this graphics item using the given painter.
   * @param painter Pointer to the QPainter object.
   * @param option Pointer to the QStyleOptionGraphicsItem object.
   * @param widget Pointer to the QWidget object.
   */
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  /**
   * @brief Retrieves the shape of this graphics item.
   * @return The QPainterPath representing the shape.
   */
  QPainterPath shape() const override;

  /**
   * @brief Retrieves the save information for this transition.
   * @return The TransitionSaveInfo object.
   */
  TransitionSaveInfo saveInfo() const;

  /**
   * @brief Retrieves the shared pointer to the storage information.
   * @return Shared pointer to the TransitionSaveInfo.
   */
  std::shared_ptr<TransitionSaveInfo> storage() const;

  /**
   * @brief Retrieves the name of this transition.
   * @return The name as a QString.
   */
  QString getName() const;

  /**
   * @brief Sets the name of this transition.
   * @param name New name for the transition.
   */
  void setName(const QString& name);

  /**
   * @brief Retrieves the event associated with this transition.
   * @return The event as a QString.
   */
  QString getEvent() const;

  /**
   * @brief Sets the event associated with this transition.
   * @param name New event for the transition.
   */
  void setEvent(const QString& name);

  /**
   * @brief Sets the type of edge for this transition.
   * @param edge The new Edge value.
   */
  void setEdge(Edge edge);

  // "signals":
  std::function<void(TransitionItem* item)> transitionDeleted;   /// Signal emitted when the transition is deleted.
  std::function<void(TransitionItem* item)> transitionModified;  /// Signal emitted when the transition is modified.

private:
  const QString mId;  /// Unique identifier for this transition.
  bool mComplete;     /// Flag indicating if the transition is complete.

  Edge mEdge = Edge::NONE;  /// Type of edge used in the transition.

  NodeItem* mSource;       /// Pointer to the source node item.
  NodeItem* mDestination;  /// Pointer to the destination node item.

  QGraphicsTextItem* mLabel = nullptr;           /// Pointer to the label text item.
  std::shared_ptr<TransitionSaveInfo> mStorage;  /// Shared pointer to the storage information.

  void updateLabelPosition();  /// Updates the position of the label text item.
};
