#include "capability_node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <oclero/qlementine.hpp>

#include "elements/flow.h"
#include "style_helpers.h"

CapabilityNode::CapabilityNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                               QGraphicsItem* parent)
    : NodeItem(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptDrops(true);
  setAcceptHoverEvents(true);
}

CapabilityNode::~CapabilityNode()
{
}

// ─────────────────────────────────────────────────────────────────────────────
// paint / shape
// ─────────────────────────────────────────────────────────────────────────────

void CapabilityNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  painter->setRenderHint(QPainter::Antialiasing, true);

  oclero::qlementine::Theme theme;
  if (auto qlementinestyle = oclero::qlementine::appStyle())
    theme = qlementinestyle->theme();

  QColor background = theme.neutralColor;
  const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());

  painter->setPen(pen);
  painter->setBrush(QBrush(background));
  const QRectF r = nodeRect().adjusted(2, 2, -2, -2);
  painter->drawEllipse(r);

  if (!config()->body.iconPath.isEmpty())
  {
    LOG_DEBUG("Rendering capability with icon: {}", config()->body.iconPath);
    paintSvg(iconPathFromTheme(config()->body.iconPath), painter, r.center(), r.width(), r.height());
  }

  NodeBase::paintLabel(painter, drawingRect(nodeRect()), pen, true);
}

QPainterPath CapabilityNode::shape() const
{
  QPainterPath path;
  path.addEllipse(nodeRect().adjusted(2, 2, -2, -2));
  return path;
}

// ─────────────────────────────────────────────────────────────────────────────
// Parent / child management
// ─────────────────────────────────────────────────────────────────────────────

void CapabilityNode::addParent(NodeItem* parent)
{
  NodeItem::addParent(parent);
  if (!parent)
    return;

  // Once a capability is parented, it is no longer movable
  setFlag(ItemIsMovable, false);
}
