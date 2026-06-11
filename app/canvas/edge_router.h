#pragma once

#include <QObject>
#include <QPainterPath>
#include <QPointF>

#include "node.h"
#include "transition.h"

class EdgeRouter : public QObject
{
  Q_OBJECT
public:
  enum class Option
  {
    DIRECT = 0,
    MANHATTAN,
    CURVED,
    OGDF,
    END
  };

  EdgeRouter(QObject* parent = nullptr);
  virtual ~EdgeRouter() = default;

  Option option() const;
  void setRouteOption(Option option);
  virtual QPainterPath route(const QPointF& sourceRect, const QPointF& destinationRect, const QList<QRectF>& obstacleRects) const;
  QHash<const TransitionItem*, QPainterPath> route(const QList<NodeItem*>& nodes, const QList<TransitionItem*>& transitions) const;

  static QString optionToString(int option);
  static QString optionToString(Option option);

private:
  Option mOption;

  QPainterPath pathFromPoints(const QVector<QPointF>& points) const;
  void appendPoint(QVector<QPointF>& points, const QPointF& point) const;

  QPainterPath curvedRoute(const QPointF& start, const QPointF& end) const;
  QVector<QPointF> directRoute(const QPointF& source, const QPointF& destination) const;
  QVector<QPointF> manhattanRoute(const QPointF& source, const QPointF& destination) const;
};