#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>
#include <QString>
#include <QVector>

#include "config.h"
#include "property_info.h"

/**
 * @brief Represents a node that can be saved and loaded.
 */
class NodeSaveInfo : public INode
{
public:
  /**
   * @brief Constructs a new NodeSaveInfo object.
   */
  NodeSaveInfo();
  NodeSaveInfo(const NodeConfig& config);

  /**
   * @brief Gets the ID of the node.
   * @return The ID as a QString.
   */
  QString getid() const override;

  /**
   * @brief Gets the node ID.
   * @return The node ID as a QString.
   */
  QString getnodeId() const override;

  /**
   * @brief Gets the position of the node.
   * @return The position as a QPointF.
   */
  QPointF getposition() const override;

  /**
   * @brief Gets the fields of the node.
   * @return A QVector of shared pointers to IProperty objects.
   */
  QVector<std::shared_ptr<IProperty>> getfields() const override;

  /**
   * @brief Gets the properties of the node.
   * @return A QMap of QString keys and QVariant values.
   */
  QMap<QString, QVariant> getproperties() const override;

  /**
   * @brief Gets the flows of the node.
   * @return A QVector of shared pointers to IFlow objects.
   */
  QVector<std::shared_ptr<IFlow>> getflows() const override;

  /**
   * @brief Gets the events of the node.
   * @return A QVector of shared pointers to IFlow objects.
   */
  QVector<std::shared_ptr<IFlow>> getevents() const override;

  /**
   * @brief Gets the parent ID of the node.
   * @return The parent ID as a QString.
   */
  QString getparentId() const override;

  /**
   * @brief Gets the children of the node.
   * @return A QVector of shared pointers to INode objects.
   */
  QVector<std::shared_ptr<INode>> getchildren() const override;

  /**
   * @brief Gets the scale of the node.
   * @return The scale as a qreal.
   */
  qreal getScale() const;

  /**
   * @brief Gets the size of the node.
   * @return The size as a QSizeF.
   */
  QSizeF getSize() const;

  /**
   * @brief Gets the pixmap of the node.
   * @return The pixmap as a QPixmap.
   */
  QPixmap getPixmap() const;

  /**
   * @brief Gets the icon path of the node.
   * @return The icon path as a QString.
   */
  QString getIcon() const;

  /**
   * @brief Sets the ID of the node.
   * @param arg The new ID as a QString.
   */
  void setId(const QString& arg);

  /**
   * @brief Sets the node ID.
   * @param arg The new node ID as a QString.
   */
  void setNodeId(const QString& arg);

  /**
   * @brief Sets the parent ID of the node.
   * @param arg The new parent ID as a QString.
   */
  void setParentId(const QString& arg);

  /**
   * @brief Sets the position of the node.
   * @param arg The new position as a QPointF.
   */
  void setPosition(const QPointF& arg);

  /**
   * @brief Sets the pixmap of the node.
   * @param arg The new pixmap as a QPixmap.
   */
  void setPixmap(const QPixmap& arg);

  /**
   * @brief Sets the icon path of the node.
   * @param iconPath The new icon path as a QString.
   */
  void setIcon(const QString& iconPath);

  /**
   * @brief Sets the size of the node.
   * @param arg The new size as a QSizeF.
   */
  void setSize(const QSizeF& arg);

  /**
   * @brief Sets the scale of the node.
   * @param arg The new scale as a qreal.
   */
  void setScale(qreal arg);

  /**
   * @brief Gets a property by key.
   * @param key The property key as a QString.
   * @return The property value as a QVariant.
   */
  QVariant getProperty(const QString& key) const;

  /**
   * @brief Adds a property to the node.
   * @param key The property key as a QString.
   * @param value The property value as a QVariant.
   */
  void addProperty(const QString& key, const QVariant& value);

  /**
   * @brief Removes a property from the node.
   * @param key The property key as a QString.
   */
  void removeProperty(const QString& key);

  /**
   * @brief Gets a field by key.
   * @param key The field key as a QString.
   * @return The field as a PropertyInfo object.
   */
  PropertyInfo getField(const QString& key) const;

  /**
   * @brief Sets a field for the node.
   * @param key The field key as a QString.
   * @param property The new field as a shared pointer to IProperty.
   */
  void setField(const QString& key, std::shared_ptr<IProperty> property);

  /**
   * @brief Adds a field to the node.
   * @param property The new field as a shared pointer to IProperty.
   */
  void addField(std::shared_ptr<IProperty> property);

  /**
   * @brief Removes a field from the node.
   * @param key The field key as a QString.
   */
  void removeField(const QString& key);

  /**
   * @brief Adds a flow to the node.
   * @param flow The new flow as a shared pointer to IFlow.
   */
  void addFlow(std::shared_ptr<IFlow> flow);

  /**
   * @brief Removes a flow from the node by reference.
   * @param flow The flow to be removed.
   */
  void removeFlow(std::shared_ptr<IFlow> flow);

  /**
   * @brief Removes a flow from the node by ID.
   * @param flowId The ID of the flow to remove as a QString.
   */
  void removeFlow(const QString& flowId);

  /**
   * @brief Adds a flow to the node.
   * @param flow The new flow as a shared pointer to IFlow.
   */
  void addEvent(std::shared_ptr<IFlow> event);

  /**
   * @brief Removes a flow from the node by reference.
   * @param flow The flow to be removed.
   */
  void removeEvent(std::shared_ptr<IFlow> event);

  /**
   * @brief Adds a child to the node.
   * @param child The new child as a shared pointer to INode.
   */
  void addChild(std::shared_ptr<INode> child);

  /**
   * @brief Removes a child from the node by ID.
   * @param childId The ID of the child to remove as a QString.
   */
  void removeChild(const QString& childId);

  /**
   * @brief Clears all children from the node.
   */
  void clearChildren();

  /**
   * @brief Converts the node to a JSON object.
   * @return A QJsonObject representing the node.
   */
  QJsonObject toJson() const;

  /**
   * @brief Creates a NodeSaveInfo object from a JSON object.
   * @param data The QJsonObject containing the node data.
   * @return A new NodeSaveInfo object.
   */
  static NodeSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info);

private:
  QString mId;        /// The ID of the node.
  QString mNodeId;    /// The node ID.
  QString mParentId;  /// The parent ID of the node.

  QPointF mPosition;  /// The position of the node.
  QSizeF mSize;       /// The size of the node.
  qreal mScale;       /// The scale of the node.

  QPixmap mPixmap;    /// The pixmap of the node.
  QString mIconPath;  /// The icon path of the node.

  QMap<QString, QVariant> mProperties;                 /// A map of properties for the node.
  QVector<std::shared_ptr<INode>> mChildren;           /// A vector of children nodes.
  QVector<std::shared_ptr<IFlow>> mFlows;              /// A vector of flows associated with the node.
  QVector<std::shared_ptr<IFlow>> mEvents;             /// A vector of flows associated with the node.
  QVector<std::shared_ptr<ITransition>> mTransitions;  /// A vector of transitions for the node.
  QVector<std::shared_ptr<IProperty>> mFields;         /// A vector of fields for the node.
};

/**
 * @brief Overloads the << operator to serialize a QVector of shared pointers to INode objects.
 * @param out The QDataStream to write to.
 * @param nodes The QVector of shared pointers to INode objects to serialize.
 * @return The modified QDataStream.
 */
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<INode>>& nodes);

/**
 * @brief Overloads the >> operator to deserialize a QVector of shared pointers to INode objects.
 * @param in The QDataStream to read from.
 * @param nodes The QVector of shared pointers to INode objects to deserialize.
 * @return The modified QDataStream.
 */
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<INode>>& nodes);

/**
 * @brief Overloads the << operator to serialize a QVector of shared pointers to NodeSaveInfo objects.
 * @param out The QDataStream to write to.
 * @param nodes The QVector of shared pointers to NodeSaveInfo objects to serialize.
 * @return The modified QDataStream.
 */
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);

/**
 * @brief Overloads the >> operator to deserialize a QVector of shared pointers to NodeSaveInfo objects.
 * @param in The QDataStream to read from.
 * @param nodes The QVector of shared pointers to NodeSaveInfo objects to deserialize.
 * @return The modified QDataStream.
 */
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
