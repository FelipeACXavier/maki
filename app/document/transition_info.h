#pragma once

#include <QPointF>
#include <QString>

#include "idocument.h"

class QJsonObject;

/**
 * @brief Represents transition save information.
 */
class TransitionSaveInfo : public ITransition
{
public:
  /**
   * @brief Constructs a new TransitionSaveInfo object.
   */
  TransitionSaveInfo();

  /**
   * @brief Gets the ID of the transition.
   * @return QString The ID of the transition.
   */
  QString getid() const override;

  /**
   * @brief Gets the label of the transition.
   * @return QString The label of the transition.
   */
  QString getlabel() const override;

  /**
   * @brief Gets the event associated with the transition.
   * @return QString The event associated with the transition.
   */
  QString getevent() const override;

  /**
   * @brief Gets the source ID of the transition.
   * @return QString The source ID of the transition.
   */
  QString getsrcId() const override;

  /**
   * @brief Gets the destination ID of the transition.
   * @return QString The destination ID of the transition.
   */
  QString getdstId() const override;

  /**
   * @brief Gets the source point of the transition.
   * @return QPointF The source point of the transition.
   */
  QPointF srcPoint() const;

  /**
   * @brief Gets the source shift of the transition.
   * @return QPointF The source shift of the transition.
   */
  QPointF srcShift() const;

  /**
   * @brief Gets the destination point of the transition.
   * @return QPointF The destination point of the transition.
   */
  QPointF dstPoint() const;

  /**
   * @brief Gets the destination shift of the transition.
   * @return QPointF The destination shift of the transition.
   */
  QPointF dstShift() const;

  /**
   * @brief Sets the ID of the transition.
   * @param arg The new ID for the transition.
   */
  void setId(const QString& arg);

  /**
   * @brief Sets the label of the transition.
   * @param arg The new label for the transition.
   */
  void setLabel(const QString& arg);

  /**
   * @brief Sets the event associated with the transition.
   * @param arg The new event for the transition.
   */
  void setEvent(const QString& arg);

  /**
   * @brief Sets the source ID of the transition.
   * @param arg The new source ID for the transition.
   */
  void setSrcId(const QString& arg);

  /**
   * @brief Sets the source point of the transition.
   * @param arg The new source point for the transition.
   */
  void setSrcPoint(const QPointF& arg);

  /**
   * @brief Sets the source shift of the transition.
   * @param arg The new source shift for the transition.
   */
  void setSrcShift(const QPointF& arg);

  /**
   * @brief Sets the destination ID of the transition.
   * @param arg The new destination ID for the transition.
   */
  void setDstId(const QString& arg);

  /**
   * @brief Sets the destination point of the transition.
   * @param arg The new destination point for the transition.
   */
  void setDstPoint(const QPointF& arg);

  /**
   * @brief Sets the destination shift of the transition.
   * @param arg The new destination shift for the transition.
   */
  void setDstShift(const QPointF& arg);

  /**
   * @brief Converts the transition save info to a JSON object.
   * @return QJsonObject The JSON representation of the transition save info.
   */
  QJsonObject toJson() const;

  /**
   * @brief Creates a TransitionSaveInfo object from a JSON object.
   * @param data The JSON object containing the transition save info.
   * @return TransitionSaveInfo The created TransitionSaveInfo object.
   */
  static TransitionSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info);

private:
  QString mId; /// The ID of the transition.
  QString mLabel; /// The label of the transition.
  QString mEvent; /// The event associated with the transition.

  QString mSrcId; /// The source ID of the transition.
  QPointF mSrcPoint; /// The source point of the transition.
  QPointF mSrcShift; /// The source shift of the transition.

  QString mDstId; /// The destination ID of the transition.
  QPointF mDstPoint; /// The destination point of the transition.
  QPointF mDstShift; /// The destination shift of the transition.
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<ITransition>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<ITransition>>& nodes);
