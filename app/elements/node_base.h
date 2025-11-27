#pragma once

#include <QGraphicsItem>
#include <QString>

#include "config.h"
#include "result.h"
#include "types.h"

class NodeBase : public QGraphicsItem
{
public:
  enum
  {
    Type = Types::BASE_NODE
  };

  NodeBase(const QString& id, const QString& nodeId, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent);

  QString id() const;
  virtual int type() const override;

  virtual QString nodeId() const;

  virtual VoidResult start();
  virtual std::shared_ptr<NodeConfig> config() const;

  virtual QRectF boundingRect() const override;
  virtual QRectF scaledRect() const;
  virtual QRectF drawingRect(const QRectF& input) const;

  virtual QPainterPath nodeShape(const QRectF& bounds) const;
  void paintNode(const QRectF& bounds, const QColor& background, const QPen& text, QPainter* painter);

  virtual QPixmap nodePixmap() const;
  virtual void toggleLabelVisibility();

protected:
  std::shared_ptr<NodeConfig> mConfig;

  std::shared_ptr<QGraphicsTextItem> mLabel;
  std::shared_ptr<QGraphicsPixmapItem> mPixmapItem;

  virtual void updateLabelPosition();
  virtual void setPixmap(const QPixmap& pixmap);
  virtual void setLabel(const QString& name, qreal fontSize);
  virtual void setLabelName(const QString& name);
  virtual void setLabelSize(qreal fontSize, const QSizeF& boundingSize);

  virtual void paintLabel(QPainter* painter, const QRectF& area) const;
  virtual void paintPixmap(QPainter* painter) const;

private:
  const QString mId;
  const QRectF mBounds;
  const QString mNodeId;

  QRectF mScaledBounds;

  qreal computeScaleFactor() const;
};
