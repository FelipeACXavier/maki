#pragma once

#include <QBrush>
#include <QDataStream>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "config.h"
// #include "inode.h"
#include "node_base.h"
#include "save_info.h"
#include "transition.h"
#include "types.h"

class Flow;
class QGraphicsSceneMouseEvent;

/**
 * @brief Represents a graphical node item in a flowchart.
 */
class NodeItem : public NodeBase
{
public:
  enum
  {
    Type = Types::NODE
  };

  /**
   * @brief Constructs a new NodeItem with the given parameters.
   *
   * @param id The unique identifier for the node.
   * @param info The save information for the node.
   * @param initialPosition The initial position of the node in scene coordinates.
   * @param nodeConfig The configuration for the node.
   * @param parent The parent graphics item, if any.
   */
  NodeItem(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
           QGraphicsItem* parent = nullptr);

  /**
   * @brief Destructor for the NodeItem.
   */
  virtual ~NodeItem();

  /**
   * @brief Returns the type of this item.
   *
   * @return The type of the item.
   */
  int type() const override;

  /**
   * @brief Starts the node.
   *
   * @return VoidResult indicating success or failure.
   */
  VoidResult start() override;

  /**
   * @brief Returns the bounding rectangle for this item.
   *
   * @return The bounding rectangle.
   */
  QRectF nodeRect() const override;
  QRectF sceneNodeRect() const;

  /**
   * @brief Returns the shape of this item.
   *
   * @return The QPainterPath representing the shape.
   */
  QPainterPath shape() const override;

  /**
   * @brief Paints the item using the given painter.
   *
   * @param painter The painter to use for drawing.
   * @param style The style option for the graphics item.
   * @param widget The parent widget, if any.
   */
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  /**
   * @brief Returns help configuration for this node.
   *
   * @return The HelpConfig object.
   */
  HelpConfig help() const;

  /**
   * @brief Returns the name of the node.
   *
   * @return The node name.
   */
  QString nodeName() const;

  /**
   * @brief Returns the type of the node.
   *
   * @return The node type.
   */
  QString nodeType() const;

  /**
   * @brief Returns the behavior of the node.
   *
   * @return The behavior string.
   */
  QString behaviour() const;

  /**
   * @brief Returns the controls configuration for this node.
   *
   * @return The QVector of ControlsConfig objects.
   */
  QVector<ControlsConfig> controls() const;

  /**
   * @brief Returns the properties of the node.
   *
   * @return The QMap of property keys to QVariant values.
   */
  QVector<std::shared_ptr<IParameter>> properties() const;

  /**
   * @brief Returns the configuration properties of the node.
   *
   * @return The QVector of PropertyConfig objects.
   */
  QVector<PropertyConfig> configurationProperties() const;

  /**
   * @brief Returns the library type associated with this node's function.
   *
   * @return The Types::LibraryTypes enum value.
   */
  Types::LibraryTypes function() const;

  /**
   * @brief Gets a property by key.
   *
   * @param key The property key.
   * @return The QVariant value of the property.
   */
  const PropertyInfo* getProperty(const QString& key) const;

  /**
   * @brief Sets a property with a given key and value.
   *
   * @param key The property key.
   * @param value The new value for the property.
   */
  void setProperty(const QString& key, const maki::Value& value);

  /**
   * @brief Renames the node to a new name.
   *
   * @param name The new name for the node.
   */
  void renameNode(const QString& name);

  /**
   * @brief Returns the parent node of this item.
   *
   * @return The parent NodeItem, if any.
   */
  NodeItem* parentNode() const;

  /**
   * @brief Returns a list of child nodes.
   *
   * @return The QVector of child NodeItems.
   */
  QVector<NodeItem*> children() const;

  /**
   * @brief Calculates an edge point toward a target scene position.
   *
   * @param targetScenePos The target scene position.
   * @return The QPointF representing the edge point.
   */
  QPointF edgePointToward(const QPointF& targetScenePos) const;

  /**
   * @brief Returns a list of fields associated with this node.
   *
   * @return The QVector of IParameter pointers.
   */
  QVector<std::shared_ptr<IParameter>> fields() const;

  /**
   * @brief Gets a property info by key.
   *
   * @param key The property key.
   * @return The PropertyInfo object for the given key.
   */
  PropertyInfo getField(const QString& key) const;

  /**
   * @brief Sets a field with a given key and JSON object.
   *
   * @param key The property key.
   * @param property The QJsonObject representing the property.
   * @return VoidResult indicating success or failure.
   */
  VoidResult setField(const QString& key, const QJsonObject& property);

  /**
   * @brief Sets a field with a given key and PropertyInfo object.
   *
   * @param key The property key.
   * @param property The PropertyInfo object to set.
   * @return VoidResult indicating success or failure.
   */
  VoidResult setField(const QString& key, std::shared_ptr<PropertyInfo> property);

  /**
   * @brief Removes a field by key.
   *
   * @param key The property key to remove.
   */
  void removeField(const QString& key);

  /**
   * @brief Sets an event at the given index with the provided FlowConfig.
   *
   * @param index The index of the event.
   * @param event The FlowConfig object for the event.
   */
  void setEvent(int index, const FlowConfig& event);

  /**
   * @brief Returns a list of events associated with this node.
   *
   * @return The QVector of IFlow pointers.
   */
  QVector<std::shared_ptr<IFlow>> events() const;

  /**
   * @brief Adds a parent node to this item.
   *
   * @param node The NodeItem to add as a parent.
   */
  void addParent(NodeItem* node);

  /**
   * @brief Adds a child node with the given save information.
   *
   * @param node The NodeItem to add as a child.
   * @param info The save information for the child node.
   */
  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info);

  /**
   * @brief Handles when a child is removed from this item.
   *
   * @param child The NodeItem that was removed.
   */
  void childRemoved(NodeItem* child);

  /**
   * @brief Returns a list of flows associated with this node.
   *
   * @return The QVector of Flow pointers.
   */
  QVector<Flow*> flows() const;

  /**
   * @brief Creates a new flow with the given name and save information.
   *
   * @param flowName The name of the flow.
   * @param info The save information for the flow.
   * @return The created Flow object, if successful.
   */
  Flow* createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info);

  /**
   * @brief Gets a flow by its ID.
   *
   * @param flowId The ID of the flow to retrieve.
   * @return The Flow object, if found.
   */
  Flow* getFlow(const QString& flowId) const;

  /**
   * @brief Deletes a flow by its ID.
   *
   * @param flowId The ID of the flow to delete.
   */
  void deleteFlow(const QString& flowId);

  /**
   * @brief Updates all flows associated with this node.
   */
  void updateFlow();

  /**
   * @brief Returns the base scale for this node.
   *
   * @return The base scale value.
   */
  qreal baseScale() const;

  /**
   * @brief Returns a list of configured transitions.
   *
   * @return The QVector of TransitionConfig objects.
   */
  QVector<TransitionConfig> configTransitions() const;

  /**
   * @brief Applies a new size to the node.
   *
   * @param size The new QSizeF for the node.
   */
  void applySize(const QSizeF& size);

  /**
   * @brief Updates the position of the node.
   *
   * @param position The new QPointF for the node's position.
   */
  void updatePosition(const QPointF& position);

  // "signals":
  std::function<void(NodeItem* item)> nodeModified;
  std::function<void(Flow* flow, NodeItem* item)> flowAdded;
  std::function<void(NodeItem* item)> nodeMoved;

  // "slots":
  void onProperties();

  // Serialization functions

  /**
   * @brief Returns the savable information for this node.
   *
   * @return A NodeSaveInfo class populated with the information from this node
   */
  NodeSaveInfo saveInfo() const;
  /**
   * @brief Saves the state of this NodeItem to a QDataStream.
   *
   * @param out The QDataStream to save to.
   * @param config The NodeItem to save.
   * @return The modified QDataStream.
   */
  friend QDataStream& operator<<(QDataStream& out, const NodeItem& config);

  /**
   * @brief Loads the state of a NodeItem from a QDataStream.
   *
   * @param in The QDataStream to load from.
   * @param config The NodeItem to load into.
   * @return The modified QDataStream.
   */
  friend QDataStream& operator>>(QDataStream& in, NodeItem& config);

protected:
  /**
   * @brief Handles mouse move events for this item.
   *
   * @param event The QGraphicsSceneMouseEvent object.
   */
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles mouse press events for this item.
   *
   * @param event The QGraphicsSceneMouseEvent object.
   */
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles mouse release events for this item.
   *
   * @param event The QGraphicsSceneMouseEvent object.
   */
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles changes to the item's properties.
   *
   * @param change The GraphicsItemChange enum value.
   * @param value The new value for the property.
   * @return The modified QVariant value.
   */
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
  std::shared_ptr<NodeSaveInfo> mStorage;  /// Save information for the node.

  QVector<Flow*> mFlows;              /// List of flows associated with this node.
  NodeItem* mParentNode;              /// Parent node of this item, if any.
  QVector<NodeItem*> mChildrenNodes;  /// List of child nodes.

  qreal mBaseScale;             /// Base scale for the node.
  QSizeF mSize{0, 0};           /// Current size of the node.
  QPointF mDragStartPos{0, 0};  /// Position where dragging started.
  QPointF mLastPosition{0, 0};  /// Last known position of the node.

  bool mIsResizing{false};             /// Flag indicating if the node is being resized.
  QPointF mResizeStartMousePos{0, 0};  /// Mouse position when resizing started.
  QSizeF mResizeStartSize{0, 0};       /// Size of the node when resizing started.

  /**
   * @brief Updates extra positions related to this node.
   */
  void updateExtrasPosition();

  /**
   * @brief Clamps a size within valid limits.
   *
   * @param width The width to clamp.
   * @param height The height to clamp.
   * @return The clamped QSizeF value.
   */
  QSizeF clampSize(qreal width, qreal height) const;

  /**
   * @brief Clamps a position inside an inner rectangle.
   *
   * @param inner The inner QRectF.
   * @param childSceneRect The scene rectangle of the child item.
   * @return The clamped QPointF value.
   */
  QPointF clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const;

  /**
   * @brief Fits this node inside its parent with optional padding.
   *
   * @param padding The padding to apply when fitting.
   */
  void fitInsideParent(qreal padding);

  /**
   * @brief Returns the inner scene rectangle of the parent, optionally padded.
   *
   * @param padding The padding to apply.
   * @return The QRectF representing the inner scene rectangle.
   */
  QRectF parentInnerSceneRect(qreal padding) const;
};
