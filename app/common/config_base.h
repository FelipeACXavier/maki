#pragma once

#include <QDataStream>
#include <QString>

/**
 * @brief Base class for configuration settings.
 */
class ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  ConfigBase();

  /**
   * @brief Copy constructor.
   * @param copy The object to copy.
   */
  ConfigBase(ConfigBase& copy);

  /**
   * @brief Copy constructor.
   * @param copy The object to copy.
   */
  ConfigBase(const ConfigBase& copy);

  /**
   * @brief Stream insertion operator for ConfigBase objects.
   * @param out The output stream.
   * @param config The configuration object to insert.
   * @return A reference to the output stream.
   */
  friend QDataStream& operator<<(QDataStream& out, const ConfigBase& config);

  /**
   * @brief Stream extraction operator for ConfigBase objects.
   * @param in The input stream.
   * @param config The configuration object to extract into.
   * @return A reference to the input stream.
   */
  friend QDataStream& operator>>(QDataStream& in, ConfigBase& config);

  /**
   * @brief Checks if the configuration is valid.
   * @return True if the configuration is valid, false otherwise.
   */
  bool isValid() const;

  /// Indicates whether the configuration is valid.
  bool isConfigValid;

  /// Error message if the configuration is invalid.
  QString errorMessage;

protected:
  /**
   * @brief Sets the configuration as invalid with an error message.
   * @param message The error message to set.
   */
  void setInvalid(const QString& message);
};
