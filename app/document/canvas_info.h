#pragma once

#include <QPointF>
#include <QSizeF>
#include <QString>

class QJsonObject;

/**
 * @brief Represents information needed to save and load a canvas state.
 */
class CanvasSaveInfo
{
public:
  /**
   * @brief Constructs a new CanvasSaveInfo object with default values.
   */
  CanvasSaveInfo();

  /**
   * @brief Returns the current scale of the canvas.
   * @return The scale value.
   */
  qreal scale() const;

  /**
   * @brief Returns the center point of the canvas.
   * @return The center point as a QPointF.
   */
  QPointF center() const;

  /**
   * @brief Sets the scale of the canvas.
   * @param scale The new scale value.
   */
  void setScale(qreal scale);

  /**
   * @brief Sets the center point of the canvas.
   * @param center The new center point as a QPointF.
   */
  void setCenter(const QPointF& center);

  /**
   * @brief Converts the CanvasSaveInfo object to a JSON object.
   * @return A QJsonObject representing the current state of the canvas.
   */
  QJsonObject toJson() const;

  /**
   * @brief Creates a CanvasSaveInfo object from a JSON object.
   * @param data The QJsonObject containing the saved canvas information.
   * @return A new CanvasSaveInfo object initialized with the data.
   */
  static CanvasSaveInfo fromJson(const QJsonObject& data);

  /**
   * @brief Overloads the << operator to serialize a CanvasSaveInfo object to a QDataStream.
   * @param out The QDataStream to write to.
   * @param info The CanvasSaveInfo object to serialize.
   * @return A reference to the modified QDataStream.
   */
  friend QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info);

  /**
   * @brief Overloads the >> operator to deserialize a CanvasSaveInfo object from a QDataStream.
   * @param in The QDataStream to read from.
   * @param info The CanvasSaveInfo object to deserialize into.
   * @return A reference to the modified QDataStream.
   */
  friend QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info);

private:
  qreal mScale = {1.0};  /// The scale of the canvas.
  QPointF mCenter = {0, 0};  /// The center point of the canvas.
};
