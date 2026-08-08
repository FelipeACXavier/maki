#include "elements/behaviour/link_out_node.h"

#include <QFontMetricsF>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "app_configs.h"
#include "system/canvas.h"

QString LinkOutNode::targetId() const
{
  const QVariant value = getProperty(link_nodes::kTargetPropertyId);
  if (!value.isValid())
    return {};
  return value.toString().trimmed();
}

QString LinkOutNode::targetName() const
{
  const QString id = targetId();
  if (id.isEmpty() || !scene())
    return {};

  for (QGraphicsItem* item : scene()->items())
  {
    if (!item || item->type() != NodeItem::Type)
      continue;
    auto* candidate = static_cast<NodeItem*>(item);
    if (candidate->id() == id)
      return candidate->nodeName();
  }

  return {};
}

void LinkOutNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  if (!painter)
    return;

  const QString name = targetName();
  if (name.isEmpty())
    return;

  const QRectF bounds = drawingRect(nodeRect());
  QFont font;
  font.setPointSizeF(qBound(6.0, qMin(bounds.width(), bounds.height()) * 0.12, 9.0));
  const QFontMetricsF fm(font);

  const QRectF textRect(bounds.left() + 4.0,
                        bounds.bottom() - fm.height() - 4.0,
                        bounds.width() - 8.0,
                        fm.height());
  if (textRect.height() <= 0.0)
    return;

  painter->setFont(font);
  painter->setPen(QPen(Config::FOREGROUND));
  painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter,
                    fm.elidedText(name, Qt::ElideRight, textRect.width()));
}

void LinkOutNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    const QString id = targetId();
    if (!id.isEmpty())
    {
      if (auto* canvas = dynamic_cast<Canvas*>(scene()))
        canvas->onFocusNode(QString(), id);
    }
  }

  QGraphicsItem::mouseDoubleClickEvent(event);
}
