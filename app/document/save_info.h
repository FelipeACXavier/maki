#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"
#include "idocument.h"

// Single header for the document stuff
#include "canvas_info.h"
#include "flow_info.h"
#include "node_info.h"
#include "property_info.h"
#include "transition_info.h"

/**
 * @brief Represents a save information object that implements IDocument.
 */
class SaveInfo : public IDocument
{
public:
  /**
   * @brief Default constructor.
   */
  SaveInfo() = default;

  /**
   * @brief Get the canvas save info.
   * @return CanvasSaveInfo The canvas save info.
   */
  CanvasSaveInfo canvasInfo() const;

  /**
   * @brief Set the canvas save info.
   * @param info The new canvas save info.
   */
  void setCanvasInfo(const CanvasSaveInfo& info);

  /**
   * @brief Get all nodes.
   * @return QVector<std::shared_ptr<INode>> A vector of shared pointers to INode objects.
   */
  QVector<std::shared_ptr<INode>> getnodes() const override;

  void clearNodes();

  /**
   * @brief Add a node.
   * @param node The node to add.
   */
  void addNode(std::shared_ptr<NodeSaveInfo> node);

  /**
   * @brief Remove a node.
   * @param node The node to remove.
   */
  void removeNode(const QString& nodeId);

  /**
   * @brief Convert the object to JSON format.
   * @return QJsonObject The JSON representation of the object.
   */
  QJsonObject toJson() const;

  /**
   * @brief Create a SaveInfo object from a JSON object.
   * @param data The JSON data.
   * @return SaveInfo The created SaveInfo object.
   */
  static SaveInfo fromJson(const QJsonObject& data);

  /**
   * @brief Serialize the object to a QDataStream.
   * @param out The output stream.
   * @param info The SaveInfo object to serialize.
   * @return QDataStream& The output stream.
   */
  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);

  /**
   * @brief Deserialize the object from a QDataStream.
   * @param in The input stream.
   * @param info The SaveInfo object to deserialize.
   * @return QDataStream& The input stream.
   */
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);

  /**
   * @brief Get possible states for a node.
   * @param nodeId The ID of the node.
   * @return QVector<std::shared_ptr<IProperty>> A vector of shared pointers to IProperty objects representing possible states.
   */
  QVector<std::shared_ptr<IProperty>> getPossibleStates(const QString& nodeId) const;

  /**
   * @brief Get possible callers for a node.
   * @param nodeId The ID of the node.
   * @return QVector<std::shared_ptr<NodeSaveInfo>> A vector of shared pointers to NodeSaveInfo objects representing possible callers.
   */
  QVector<std::shared_ptr<NodeSaveInfo>> getPossibleCallers(const QString& nodeId, const Types::PropertyTypes type) const;

  /**
   * @brief Get events from a node.
   * @param nodeId The ID of the node.
   * @return QVector<std::shared_ptr<FlowSaveInfo>> A vector of shared pointers to FlowSaveInfo objects representing events.
   */
  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId) const;

  QVector<std::shared_ptr<FlowSaveInfo>> getFlowsFromNode(const QString& nodeId) const;

  /**
   * @brief Get events of a specific type from a node.
   * @param nodeId The ID of the node.
   * @param type The type of event.
   * @return QVector<std::shared_ptr<FlowSaveInfo>> A vector of shared pointers to FlowSaveInfo objects representing events of the specified type.
   */
  QVector<std::shared_ptr<FlowSaveInfo>> getEventsOfTypeFromNode(const QString& nodeId, const QList<Types::CallType>& types) const;

  /**
   * @brief Get an event from a node by name.
   * @param nodeId The ID of the node.
   * @param flowName The name of the event.
   * @return std::shared_ptr<FlowSaveInfo> A shared pointer to FlowSaveInfo object representing the event.
   */
  std::shared_ptr<FlowSaveInfo> getEventFromNode(const QString& nodeId, const QString& flowName) const;

  std::shared_ptr<FlowSaveInfo> getFlowFromNode(const QString& nodeId, const QString& flowName) const;

  /**
   * @brief Get a node by ID.
   * @param nodeId The ID of the node.
   * @return std::shared_ptr<NodeSaveInfo> A shared pointer to NodeSaveInfo object representing the node.
   */
  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId);

  /**
   * @brief Get a flow by ID.
   * @param flowId The ID of the flow.
   * @return std::shared_ptr<FlowSaveInfo> A shared pointer to FlowSaveInfo object representing the flow.
   */
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId);

  QVector<std::shared_ptr<FlowSaveInfo>> pipelines() const;

  void addPipeline(std::shared_ptr<FlowSaveInfo> pipeline);

  QString name;
  QString version;
  QString rootPath;
  QString saveFile;

private:
  CanvasSaveInfo mCanvasInfo;                         /// The canvas save info.
  QVector<std::shared_ptr<INode>> mStructuralNodes;   /// A vector of shared pointers to INode objects representing structural nodes.
  QVector<std::shared_ptr<FlowSaveInfo>> mPipelines;  /// A vector of shared pointers to IFlow objects representing the available pipelines.

  /**
   * @brief Find the family of a construct.
   * @param nodeId The ID of the node.
   * @param nodes The vector of nodes to search.
   * @return QVector<std::shared_ptr<NodeSaveInfo>> A vector of shared pointers to NodeSaveInfo objects representing the family of the construct.
   */
  QVector<std::shared_ptr<NodeSaveInfo>> findFamilyOfFlowNode(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes, const Types::PropertyTypes type) const;

  void findChildrenOfTask(const std::shared_ptr<INode> task, QVector<std::shared_ptr<NodeSaveInfo>>& nodes, const Types::PropertyTypes type) const;

  /**
   * @brief Find the parent of a construct.
   * @param nodeId The ID of the node.
   * @param node The node to search.
   * @return std::shared_ptr<NodeSaveInfo> A shared pointer to NodeSaveInfo object representing the parent of the construct.
   */
  std::shared_ptr<NodeSaveInfo> findOwnerTaskOfFlowNode(const QString& nodeId, const std::shared_ptr<INode> node) const;

  /**
   * @brief Get events from a node.
   * @param nodeId The ID of the node.
   * @param nodes The vector of nodes to search.
   * @return QVector<std::shared_ptr<FlowSaveInfo>> A vector of shared pointers to FlowSaveInfo objects representing events.
   */
  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const;

  QVector<std::shared_ptr<FlowSaveInfo>> getFlowsFromNode(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const;

  /**
   * @brief Get a node by ID.
   * @param nodeId The ID of the node.
   * @param nodes The vector of nodes to search.
   * @return std::shared_ptr<NodeSaveInfo> A shared pointer to NodeSaveInfo object representing the node.
   */
  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes);

  /**
   * @brief Get a flow by ID.
   * @param flowId The ID of the flow.
   * @param nodes The vector of nodes to search.
   * @return std::shared_ptr<FlowSaveInfo> A shared pointer to FlowSaveInfo object representing the flow.
   */
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId, const QVector<std::shared_ptr<INode>>& nodes);
};
