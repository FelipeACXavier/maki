#pragma once

#include <QJsonObject>
#include <QList>

#include "idocument.h"
#include "types.h"

class PropertyConfig;

/**
 * @brief Represents a property with configuration and options.
 */
class PropertyInfo : public IProperty
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

  /**
   * @brief Gets the default value of the property.
   *
   * @return QVariant The default value of the property.
   */
  QVariant getdefaultValue() const override;

  /**
   * @brief Gets the options available for this property.
   *
   * @return QVector<std::shared_ptr<IProperty>> A vector of options.
   */
  QVector<std::shared_ptr<IProperty>> getoptions() const override;

  /**
   * @brief Gets the type of the property.
   *
   * @return Types::PropertyTypes The type of the property.
   */
  Types::PropertyTypes gettype() const override;

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
  void setDefaultValue(const QVariant& arg);

  /**
   * @brief Sets the type of the property.
   *
   * @param arg The new type for the property.
   */
  void setType(Types::PropertyTypes arg);

  /**
   * @brief Gets a specific option by ID.
   *
   * @param optionId The ID of the option to retrieve.
   * @return PropertyInfo The option with the specified ID.
   */
  PropertyInfo getOption(const QString& optionId);

  /**
   * @brief Adds an option to this property.
   *
   * @param option The option to add.
   */
  void addOption(std::shared_ptr<IProperty> option);

  /**
   * @brief Removes an option from this property.
   *
   * @param option The option to remove.
   */
  void removeOption(std::shared_ptr<IProperty> option);

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
  QString mId; /// ID of the property
  QVariant mDefaultValue; /// Default value of the property
  QVector<std::shared_ptr<IProperty>> mOptions; /// Options available for this property
  Types::PropertyTypes mType; /// Type of the property
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<PropertyInfo>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<PropertyInfo>>& properties);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IProperty>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IProperty>>& properties);
