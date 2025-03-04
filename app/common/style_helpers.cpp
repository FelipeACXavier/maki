#include "style_helpers.h"

#include <QStyle>
#include <QVariant>

#include "string_helpers.h"

static const char* CLASS = "class";

void addClass(QWidget* widget, const QString& className)
{
  // Append the new class if there is already a class, otherwise just add it
  QVariant variant = widget->property(CLASS);
  QString classList = variant.isValid() ? QStringLiteral("%1 %2").arg(variant.toString(), className) : className;

  widget->setProperty(CLASS, QVariant(classList));
}

QPointF snapToGrid(const QPointF& point, const int gridSize)
{
  qreal snappedX = std::round(point.x() / gridSize) * gridSize;
  qreal snappedY = std::round(point.y() / gridSize) * gridSize;

  return QPointF(snappedX, snappedY);
}

QString ToLabel(const std::string& str)
{
  return ToLabel(QString::fromStdString(str));
}

QString ToLabel(const QString& str)
{
  return QString::fromStdString(ToUpperCase(str.toStdString(), 0, 1));
}
