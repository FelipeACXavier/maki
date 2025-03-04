#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "config.h"
#include "node_base.h"
#include "types.h"

class Connector;
class ConnectionItem;
class QGraphicsSceneMouseEvent;

class NodeItem : public NodeBase
{
public:
  enum
  {
    Type = UserType + Types::NODE
  };

  NodeItem(const QPointF& initialPosition, const QPixmap& map, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);
  virtual ~NodeItem();

  // QString id() const;
  int type() const override;

  VoidResult start() override;

  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  HelpConfig help() const;
  QString nodeType() const;
  QString behaviour() const;
  QVector<ControlsConfig> controls() const;
  QVector<PropertiesConfig> properties() const;
  QVector<PropertiesConfig> fields() const;
  QVector<std::shared_ptr<Connector>> connectors() const;

  Result<QVariant> getProperty(const QString& key);
  void setProperty(const QString& key, QVariant value);

  Result<PropertiesConfig> getField(const QString& key);
  VoidResult setField(const QString& key, const QJsonObject& value);

  std::function<void(NodeItem* item)> nodeSeletected;

  void onDelete();
  void onProperties();

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
  QVector<std::shared_ptr<Connector>> mConnectors;

  std::map<QString, QVariant> mProperties;
  QVector<PropertiesConfig> mFields;

  void updateConnectors();
};
