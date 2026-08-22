#pragma once

#include <QJsonObject>
#include <QList>

#include "idocument.h"
#include "mission_parameter.h"
#include "types.h"

class PropertyConfig;

/**
 * @brief Represents a property with configuration and options.
 */
class PropertyInfo : public IParameter
{
public:
  /**
   * @brief Constructs an empty PropertyInfo object.
   */
  PropertyInfo();

  /**
   * @brief Constructs a PropertyInfo object from a PropertyConfig.
   *
   * @param object The PropertyConfig to initialize the object with.
   */
  PropertyInfo(const PropertyConfig& object);

  /**
   * @brief Gets the ID of the property.
   *
   * @return QString The ID of the property.
   */
  QString getid() const override;

  QString getname() const override;

  /**
   * @brief Gets the default value of the property.
   *
   * @return QVariant The default value of the property.
   */
  const IValue* getvalue() const override;

  /**
   * @brief Gets the type of the property.
   *
   * @return Types::PropertyTypes The type of the property.
   */
  koda::types::TypeReference gettype() const override;

  /**
   * @brief Gets the control type of the property.
   *
   * @return Types::ControlTypes The control type of the property.
   */
  Types::ControlTypes getcontrol() const override;

  /**
   * @brief Sets the ID of the property.
   *
   * @param arg The new ID for the property.
   */
  void setId(const QString& arg);

  /**
   * @brief Sets the default value of the property.
   *
   * @param arg The new default value for the property.
   */
  void setDefaultValue(const maki::Value& arg);

  /**
   * @brief Sets the type of the property.
   *
   * @param arg The new type for the property.
   */
  void setType(const koda::types::TypeReference& arg);

  /**
   * @brief Sets the control type of the property.
   *
   * @param arg The new control type for the property.
   */
  void setControl(Types::ControlTypes arg);

  /**
   * @brief Gets a specific option by ID.
   *
   * @param optionId The ID of the option to retrieve.
   * @return PropertyInfo The option with the specified ID.
   */
  PropertyInfo getOption(const QString& optionId);

  /**
   * @brief Converts the PropertyInfo object to a JSON object.
   *
   * @return QJsonObject A JSON representation of the object.
   */
  QJsonObject toJson() const;

  /**
   * @brief Creates a PropertyInfo object from a JSON object.
   *
   * @param data The JSON data to parse.
   * @return PropertyInfo The created PropertyInfo object.
   */
  static PropertyInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const PropertyInfo& config);
  friend QDataStream& operator>>(QDataStream& in, PropertyInfo& config);

private:
  QString mId;                       /// ID of the property
  maki::Value mDefaultValue;         /// Default value of the property
  koda::types::TypeReference mType;  /// Type of the property
  Types::ControlTypes mControlType;  /// Control type of the property
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<PropertyInfo>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<PropertyInfo>>& properties);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IParameter>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IParameter>>& properties);
