#pragma once

#include <QVector>

#include "elements/behaviour/behaviour_node.h"

namespace link_nodes
{
inline const QString kLinkInType = QStringLiteral("Koda::Link in");
inline const QString kLinkOutType = QStringLiteral("Koda::Link out");
inline const QString kTargetPropertyId = QStringLiteral("target");
}  // namespace link_nodes

class LinkOutNode : public BehaviourNode
{
public:
  struct LinkInTarget
  {
    QString id;
    QString name;
  };

  LinkOutNode(const QString& id,
              std::shared_ptr<NodeSaveInfo> info,
              const QPointF& initialPosition,
              std::shared_ptr<NodeConfig> nodeConfig,
              QGraphicsItem* parent = nullptr);

  void setProperty(const QString& key, QVariant value) override;

  /** Resolved Link in node id from the target property, or empty if unset. */
  QString targetId() const;

  /** Display name of the target Link in in this scene, or empty if unresolved. */
  QString targetName() const;

  /** Link in nodes in this container and in enclosing flows (outside a subflow, still on the same flow). */
  QVector<LinkInTarget> linkInTargets() const;

  void assignTarget(const QString& linkInId);

  QRectF chevronSceneRect() const;
  bool chevronContainsScenePoint(const QPointF& scenePos) const;

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
  void syncLinkOutLabel();
  QRectF chevronLocalRect() const;
  void setChevronHovered(bool hovered);

  bool mChevronHovered = false;
};
