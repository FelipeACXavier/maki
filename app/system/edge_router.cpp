#include "edge_router.h"

#include <QObject>

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