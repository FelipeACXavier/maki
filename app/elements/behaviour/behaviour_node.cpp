#include "elements/behaviour/behaviour_node.h"

#include <QFileInfo>
#include <QPainter>
#include <QSvgRenderer>

#include "elements/behaviour/component_overlay.h"
#include "keys.h"
#include "logging.h"
#include "style_helpers.h"
#include "system/canvas.h"
#include "system/config_table.h"
#include "types.h"

namespace
{
bool isNearDimension(qreal value, qreal reference, qreal factor)
{
  return qAbs(value - reference * factor) < 1.0;
}

QSizeF normalizedBehaviourNodeSize(const NodeConfig* cfg, const QSizeF& saved)
{
  const qreal cfgW = static_cast<qreal>(cfg->body.width);
  const qreal cfgH = static_cast<qreal>(cfg->body.height);
  const QSizeF configSize(cfgW, cfgH);

  if (saved.width() <= 0.0 || saved.height() <= 0.0)
    return configSize;

  const bool legacyHalfWidth = isNearDimension(saved.width(), cfgW, 0.5);
  const bool legacyHalfHeight = isNearDimension(saved.height(), cfgH, 0.5);
  const bool legacyDoubleWidth = isNearDimension(saved.width(), cfgW, 2.0);
  const bool legacyDoubleHeight = isNearDimension(saved.height(), cfgH, 2.0);

  if ((legacyHalfWidth && legacyHalfHeight) || (legacyHalfWidth && qAbs(saved.height() - cfgH) < 1.0) ||
      (legacyHalfHeight && qAbs(saved.width() - cfgW) < 1.0) || legacyDoubleWidth || legacyDoubleHeight)
    return configSize;

  return saved;
}
}  // namespace

BehaviourNode::BehaviourNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeItem(id, info, initialPosition, nodeConfig, parent)
{
}

void BehaviourNode::initializeNodeSize()
{
  mSize = normalizedBehaviourNodeSize(config().get(), mSize);
  mStorage->setSize(mSize);
}

void BehaviourNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  paintDefaultNode(painter, style, widget);
  paintBehaviourExtras(painter, style, widget);
}

void BehaviourNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(painter);
  Q_UNUSED(style);
  Q_UNUSED(widget);
}

void BehaviourNode::onTransitionAdded(TransitionItem* transition)
{
  Q_UNUSED(transition);
}
