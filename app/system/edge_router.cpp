#include "edge_router.h"

#include <libavoid/connector.h>
#include <libavoid/router.h>
#include <libavoid/shape.h>

QString EdgeRouter::optionToString(int option)
{
  return optionToString((EdgeRouter::Option)option);
}

QString EdgeRouter::optionToString(Option option)
{
  switch (option)
  {
    case Option::MANHATTAN:
      return "Manhattan";
    case Option::OGDF:
      return "OGDF";
    case Option::CURVED:
      return "Curved";
    case Option::DIRECT:
    default:
      return "Direct";
  }
}

EdgeRouter::EdgeRouter(QObject* parent)
    : QObject(parent)
    , mOption(Option::DIRECT)
{
}

EdgeRouter::Option EdgeRouter::option() const
{
  return mOption;
}

void EdgeRouter::setRouteOption(Option option)
{
  mOption = option;
}

QPainterPath EdgeRouter::route(const QPointF& source, const QPointF& destination, const QList<QRectF>& obstacleRects) const
{
  Q_UNUSED(obstacleRects)

  switch (mOption)
  {
    case Option::MANHATTAN:
      return pathFromPoints(manhattanRoute(source, destination));
    case Option::CURVED:
      return curvedRoute(source, destination);
      break;
    case Option::OGDF:
    case Option::DIRECT:
    default:
      return pathFromPoints(directRoute(source, destination));
  }
}

QVector<QPointF> EdgeRouter::directRoute(const QPointF& start, const QPointF& end) const
{
  return {start, end};
}

QVector<QPointF> EdgeRouter::manhattanRoute(const QPointF& start, const QPointF& end) const
{
  QVector<QPointF> points;
  points << start;

  const qreal dx = qAbs(end.x() - start.x());
  const qreal dy = qAbs(end.y() - start.y());

  if (dx > dy)
  {
    const qreal midX = (start.x() + end.x()) / 2.0;
    appendPoint(points, QPointF(midX, start.y()));
    appendPoint(points, QPointF(midX, end.y()));
  }
  else
  {
    const qreal midY = (start.y() + end.y()) / 2.0;
    appendPoint(points, QPointF(start.x(), midY));
    appendPoint(points, QPointF(end.x(), midY));
  }

  points << end;

  return points;
}

QPainterPath EdgeRouter::curvedRoute(const QPointF& start, const QPointF& end) const
{
  QPainterPath path(start);

  const QPointF mid = (start + end) / 2.0;
  const QLineF line(start, end);

  const qreal distance = line.length();
  const qreal offset = qMin(80.0, distance * 0.35);

  const QPointF normal(-line.dy() / distance, line.dx() / distance);
  const QPointF control = mid + normal * offset;

  path.quadTo(control, end);
  return path;
}

void EdgeRouter::appendPoint(QVector<QPointF>& points, const QPointF& point) const
{
  if (points.isEmpty() || QLineF(points.last(), point).length() > 0.5)
    points << point;
}

QPainterPath EdgeRouter::pathFromPoints(const QVector<QPointF>& points) const
{
  QPainterPath path;

  if (points.isEmpty())
    return path;

  path.moveTo(points.first());

  for (int i = 1; i < points.size(); ++i)
    path.lineTo(points[i]);

  return path;
}

static Avoid::Polygon polygonFromRect(const QRectF& rect, qreal margin = 12.0)
{
  QRectF r = rect.adjusted(-margin, -margin, margin, margin);

  Avoid::Polygon polygon(4);
  polygon.ps[0] = Avoid::Point(r.left(), r.top());
  polygon.ps[1] = Avoid::Point(r.right(), r.top());
  polygon.ps[2] = Avoid::Point(r.right(), r.bottom());
  polygon.ps[3] = Avoid::Point(r.left(), r.bottom());

  return polygon;
}

QHash<const TransitionItem*, QPainterPath> EdgeRouter::route(const QList<NodeItem*>& nodes, const QList<TransitionItem*>& transitions) const
{
  Avoid::Router router(Avoid::PolyLineRouting | Avoid::OrthogonalRouting);

  router.setRoutingPenalty(Avoid::segmentPenalty);
  router.setRoutingPenalty(Avoid::crossingPenalty);
  router.setRoutingPenalty(Avoid::fixedSharedPathPenalty);
  router.setRoutingPenalty(Avoid::portDirectionPenalty);

  QHash<const NodeItem*, Avoid::ShapeRef*> shapeMap;
  QHash<const TransitionItem*, Avoid::ConnRef*> connectorMap;

  for (const NodeItem* node : nodes)
  {
    QRectF rect = node->sceneNodeRect();
    auto poly = polygonFromRect(rect);
    shapeMap[node] = new Avoid::ShapeRef(&router, poly);
  }

  for (const TransitionItem* transition : transitions)
  {
    const NodeItem* source = transition->source();
    const NodeItem* target = transition->destination();

    if (!source || !target)
      continue;

    const auto sourceRect = source->sceneNodeRect();

    auto* conn = new Avoid::ConnRef(&router);
    if (!transition->getEvent().isEmpty() && option() == Option::MANHATTAN)
    {
      if (transition->getEvent() == "on error")
        conn->setSourceEndpoint(Avoid::ConnEnd(Avoid::Point(sourceRect.center().x(), sourceRect.top()),
                                               Avoid::ConnDirUp));
      else if (transition->getEvent() == "on abort")
        conn->setSourceEndpoint(Avoid::ConnEnd(Avoid::Point(sourceRect.center().x(), sourceRect.bottom()),
                                               Avoid::ConnDirDown));
      else
        conn->setSourceEndpoint(Avoid::ConnEnd(Avoid::Point(sourceRect.right(), sourceRect.center().y()),
                                               Avoid::ConnDirRight));
    }
    else
    {
      conn->setSourceEndpoint(Avoid::ConnEnd(Avoid::Point(sourceRect.right(), sourceRect.center().y()),
                                             Avoid::ConnDirRight));
    }

    const QPointF destScene = target->incomingPortAnchor();
    conn->setDestEndpoint(Avoid::ConnEnd(Avoid::Point(destScene.x(), destScene.y()), Avoid::ConnDirLeft));

    if (option() == Option::MANHATTAN)
      conn->setRoutingType(Avoid::ConnType_Orthogonal);
    else
      conn->setRoutingType(Avoid::ConnType_PolyLine);

    connectorMap[transition] = conn;
  }

  router.processTransaction();

  QHash<const TransitionItem*, QPainterPath> result;

  for (auto it = connectorMap.constBegin(); it != connectorMap.constEnd(); ++it)
  {
    const TransitionItem* transition = it.key();
    const Avoid::PolyLine& route = it.value()->displayRoute();

    QVector<QPointF> points;
    for (size_t i = 0; i < route.size(); ++i)
      points << QPointF(route.ps[i].x, route.ps[i].y);

    result[transition] = pathFromPoints(points);
  }

  return result;
}
