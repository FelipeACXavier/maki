#pragma once

#include <QString>
#include <QVector>

#include "config.h"
#include "idocument.h"

class NodeSaveInfo;

/**
 * @brief Represents the save information for a flow.
 */
class FlowSaveInfo : public IFlow
{
public:
  /**
   * @brief Constructs an empty FlowSaveInfo object.
   */
  FlowSaveInfo();

  /**
   * @brief Constructs a FlowSaveInfo object with the given configuration.
   *
   * @param config The configuration to initialize the flow save info with.
   */
  FlowSaveInfo(const FlowConfig& config);

  /**
   * @brief Gets the ID of the flow.
   *
   * @return QString The ID of the flow.
   */
  QString getid() const override;

  /**
   * @brief Gets the name of the flow.
   *
   * @return QString The name of the flow.
   */
  QString getname() const override;

  /**
   * @brief Gets the owner of the flow.
   *
   * @return QString The owner of the flow.
   */
  QString getowner() const override;

  /**
   * @brief Checks if the flow is modifiable.
   *
   * @return bool True if the flow is modifiable, false otherwise.
   */
  bool getmodifiable() const override;

  /**
   * @brief Gets the number of links to the flow.
   *
   * @return int The number of links to the flow.
   */
  int getlinksTo() const override;

  /**
   * @brief Gets the type of the call.
   *
   * @return Types::CallType The type of the call.
   */
  Types::CallType gettype() const override;

  /**
   * @brief Gets the return type of the property.
   *
   * @return Types::PropertyTypes The return type of the property.
   */
  Types::PropertyTypes getreturnType() const override;

  /**
   * @brief Gets the arguments of the flow.
   *
   * @return QVector<std::shared_ptr<IParameter>> A vector of shared pointers to IParameter objects representing the arguments.
   */
  QVector<std::shared_ptr<IParameter>> getarguments() const override;

  /**
   * @brief Gets the nodes of the flow.
   *
   * @return QVector<std::shared_ptr<INode>> A vector of shared pointers to INode objects representing the nodes.
   */
  QVector<std::shared_ptr<INode>> getnodes() const override;

  /**
   * @brief Gets the transitions of the flow.
   * @return A QVector of shared pointers to ITransition objects.
   */
  QVector<std::shared_ptr<ITransition>> gettransitions() const override;

  /**
   * @brief Gets the transitions of the flow coming from a specific node.
   * @param id The id of the node of interest.
   * @return A QVector of shared pointers to ITransition objects.
   */
  QVector<std::shared_ptr<ITransition>> gettransitions(const QString& id) const override;

  void addTransition(std::shared_ptr<ITransition> transition);

  void removeTransition(std::shared_ptr<ITransition> transition);

  /**
   * @brief Sets the ID of the flow.
   *
   * @param arg The new ID for the flow.
   */
  void setId(const QString& arg);

  /**
   * @brief Sets the name of the flow.
   *
   * @param arg The new name for the flow.
   */
  void setName(const QString& arg);

  /**
   * @brief Sets the owner of the flow.
   *
   * @param arg The new owner for the flow.
   */
  void setOwner(const QString& arg);

  /**
   * @brief Sets whether the flow is modifiable.
   *
   * @param arg True if the flow should be modifiable, false otherwise.
   */
  void setModifiable(bool arg);

  /**
   * @brief Sets the number of links to the flow.
   *
   * @param arg The new number of links for the flow.
   */
  void setLinksTo(int arg);

  /**
   * @brief Sets the type of the call.
   *
   * @param arg The new type of the call.
   */
  void setType(Types::CallType arg);

  /**
   * @brief Sets the return type of the property.
   *
   * @param arg The new return type for the property.
   */
  void setReturnType(Types::PropertyTypes arg);

  /**
   * @brief Adds an argument to the flow.
   *
   * @param arg A shared pointer to an IParameter object representing the argument to add.
   */
  void addArgument(std::shared_ptr<IParameter> arg);

  /**
   * @brief Sets an argument at a specific index in the flow.
   *
   * @param index The index of the argument to set.
   * @param arg A shared pointer to an IParameter object representing the new argument.
   */
  void setArgument(uint32_t index, std::shared_ptr<IParameter> arg);

  /**
   * @brief Gets an argument at a specific index in the flow.
   *
   * @param index The index of the argument to get.
   * @return std::shared_ptr<IParameter> A shared pointer to the IParameter object representing the argument.
   */
  std::shared_ptr<IParameter> getArgument(uint32_t index);

  /**
   * @brief Removes an argument from the flow.
   *
   * @param arg A shared pointer to an IParameter object representing the argument to remove.
   */
  void removeArgument(std::shared_ptr<IParameter> arg);

  /**
   * @brief Adds a node to the flow.
   *
   * @param arg A shared pointer to an INode object representing the node to add.
   */
  void addNode(std::shared_ptr<INode> arg);

  /**
   * @brief Removes a node from the flow.
   *
   * @param arg A shared pointer to an INode object representing the node to remove.
   */
  void removeNode(std::shared_ptr<INode> arg);

  /**
   * @brief Converts the flow save info to a JSON object.
   *
   * @return QJsonObject The JSON representation of the flow save info.
   */
  QJsonObject toJson() const;

  /**
   * @brief Creates a FlowSaveInfo object from a JSON object.
   *
   * @param data The JSON object representing the flow save info.
   * @return FlowSaveInfo A new FlowSaveInfo object initialized with the data from the JSON object.
   */
  static FlowSaveInfo fromJson(const QJsonObject& data);

  /**
   * @brief Overloads the << operator to serialize a FlowSaveInfo object to a QDataStream.
   *
   * @param out The QDataStream to write to.
   * @param info The FlowSaveInfo object to serialize.
   * @return QDataStream& A reference to the modified QDataStream.
   */
  friend QDataStream& operator<<(QDataStream& out, const FlowSaveInfo& info);

  /**
   * @brief Overloads the >> operator to deserialize a FlowSaveInfo object from a QDataStream.
   *
   * @param in The QDataStream to read from.
   * @param info The FlowSaveInfo object to deserialize into.
   * @return QDataStream& A reference to the modified QDataStream.
   */
  friend QDataStream& operator>>(QDataStream& in, FlowSaveInfo& info);

private:
  QString mId;       /// The ID of the flow.
  QString mName;     /// The name of the flow.
  QString mOwner;    /// The owner of the flow.
  bool mModifiable;  /// Whether the flow is modifiable.
  int mLinksTo;      /// The number of links to the flow.

  Types::CallType mType;                            /// The type of the call.
  Types::PropertyTypes mReturnType;                 /// The return type of the property.
  QVector<std::shared_ptr<INode>> mNodes;           /// A vector of shared pointers to INode objects representing the nodes.
  QVector<std::shared_ptr<IParameter>> mArguments;  /// A vector of shared pointers to IParameter objects representing the arguments.
  QVector<std::shared_ptr<ITransition>> mTransitions;
};

/**
 * @brief Overloads the << operator to serialize a QVector of FlowSaveInfo objects to a QDataStream.
 *
 * @param out The QDataStream to write to.
 * @param nodes The QVector of FlowSaveInfo objects to serialize.
 * @return QDataStream& A reference to the modified QDataStream.
 */
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<FlowSaveInfo>>& nodes);

/**
 * @brief Overloads the >> operator to deserialize a QVector of FlowSaveInfo objects from a QDataStream.
 *
 * @param in The QDataStream to read from.
 * @param nodes The QVector of FlowSaveInfo objects to deserialize into.
 * @return QDataStream& A reference to the modified QDataStream.
 */
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<FlowSaveInfo>>& nodes);

/**
 * @brief Overloads the << operator to serialize a QVector of IFlow objects to a QDataStream.
 *
 * @param out The QDataStream to write to.
 * @param nodes The QVector of IFlow objects to serialize.
 * @return QDataStream& A reference to the modified QDataStream.
 */
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IFlow>>& nodes);

/**
 * @brief Overloads the >> operator to deserialize a QVector of IFlow objects from a QDataStream.
 *
 * @param in The QDataStream to read from.
 * @param nodes The QVector of IFlow objects to deserialize into.
 * @return QDataStream& A reference to the modified QDataStream.
 */
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IFlow>>& nodes);
