#pragma once

#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <QString>
#include <memory>

#include "app_configs.h"
#include "config.h"

class QSvgRenderer;
#include "ids.h"
#include "result.h"

/**
 * @brief Base class for all graphical nodes.
 */
class NodeBase : public QGraphicsItem
{
public:
  enum
  {
    Type = Types::BASE_NODE
  };

  /**
   * @brief Constructs a new NodeBase instance.
   *
   * @param id Unique identifier for the node.
   * @param nodeId Identifier of the node within its type.
   * @param config Configuration settings for the node.
   * @param parent Parent QGraphicsItem.
   */
  NodeBase(const QString& id, const QString& nodeId, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent);

  /**
   * @brief Destructor.
   */
  virtual ~NodeBase();

  /**
   * @brief Retrieves the unique identifier of the node.
   *
   * @return The unique identifier.
   */
  QString id() const;

  /**
   * @brief Returns the type of the node.
   *
   * @return The type of the node.
   */
  virtual int type() const override;

  /**
   * @brief Retrieves the identifier of the node within its type.
   *
   * @return The identifier of the node within its type.
   */
  virtual QString nodeId() const;

  /**
   * @brief Starts the node.
   *
   * @return A VoidResult indicating success or failure.
   */
  virtual VoidResult start();

  /**
   * @brief Retrieves the configuration settings for the node.
   *
   * @return The configuration settings.
   */
  virtual std::shared_ptr<NodeConfig> config() const;

  /**
   * @brief Returns the bounding rectangle of the node.
   *
   * @return The bounding rectangle.
   */
  virtual QRectF boundingRect() const override;

  /**
   * @brief Returns the bounding rectangle of the label.
   *
   * @return The bounding rectangle of the label.
   */
  virtual QRectF labelBoundingRect() const;

  virtual QRectF nodeRect() const;

  /**
   * @brief Bounding rect of node body united with visible label geometry (parent item coordinates).
   *
   * Used where connectors or visuals must clear the caption drawn below the body.
   */
  QRectF itemRectIncludingLabel() const;

  /** Pixels the visible label extends below boundingRect().bottom() (0 if none). */
  qreal labelExtentBelowBody() const;

  /**
   * @brief Returns the scaled rectangle of the node.
   *
   * @return The scaled rectangle.
   */
  virtual QRectF scaledRect() const;

  /**
   * @brief Returns the drawing rectangle based on the input rectangle.
   *
   * @param input Input rectangle.
   * @return The drawing rectangle.
   */
  virtual QRectF drawingRect(const QRectF& input) const;

  /** Visible SVG shape bounds within the node body (equals drawingRect when no nodeSvg). */
  QRectF nodeShapeContentRect(const QRectF& bounds) const;

  /**
   * @brief Returns the node shape as a QPainterPath.
   *
   * @param bounds Bounding rectangle of the node.
   * @return The node shape.
   */
  virtual QPainterPath nodeShape(const QRectF& bounds) const;

  /**
   * @brief Paints the node using the provided painter.
   *
   * @param bounds Bounding rectangle of the node.
   * @param background Background color for the node.
   * @param text Text pen for the label.
   * @param painter Painter to use for drawing.
   */
  void paintNode(const QRectF& bounds, const QColor& background, const QPen& text, QPainter* painter);

  /**
   * @brief Retrieves the pixmap of the node.
   *
   * @return The pixmap of the node.
   */
  virtual QPixmap nodePixmap() const;

  /**
   * @brief Retrieves the icon path of the node.
   *
   * @return The icon path of the node.
   */
  virtual QString nodeIcon() const;

  /**
   * @brief Toggles the visibility of the label.
   */
  virtual void toggleLabelVisibility();

protected:
  std::shared_ptr<NodeConfig> mConfig;  /// Configuration settings for the node.

  QGraphicsPixmapItem* mPixmapItem = nullptr;  /// Pointer to the pixmap item.
  QString mIconPath = "";                      /// Path to the icon image.
  QGraphicsSvgItem* mIconItem = nullptr;       /// Pointer to the SVG icon item.

  /**
   * @brief Sets the pixmap for the node.
   *
   * @param pixmap The new pixmap.
   */
  virtual void setPixmap(const QPixmap& pixmap);

  /**
   * @brief Sets the icon for the node.
   *
   * @param path Path to the icon image.
   * @param iconColor Color of the icon.
   */
  virtual void setIcon(const QString& path, const QColor& iconColor);

  /**
   * @brief Sets the label text and font size.
   *
   * @param name The label text.
   * @param fontSize Font size for the label.
   */
  virtual void setLabel(const QString& name, qreal fontSize);

  /**
   * @brief Sets the label text.
   *
   * @param name The label text.
   */
  virtual void setLabelName(const QString& name);

  /**
   * @brief Sets the font size and bounding size for the label.
   *
   * @param fontSize Font size for the label.
   * @param boundingSize Bounding size for the label.
   */
  virtual void setLabelSize(qreal fontSize, const QSizeF& boundingSize);

  /**
   * @brief Paints the label using the provided painter.
   *
   * @param painter Painter to use for drawing.
   * @param area Area to paint the label in.
   */
  virtual void paintLabel(QPainter* painter, const QPen& pen) const;

  /**
   * @brief Paints the pixmap using the provided painter.
   *
   * @param painter Painter to use for drawing.
   */
  virtual void paintPixmap(QPainter* painter) const;

private:
  const QString mId;     /// Unique identifier of the node.
  const QRectF mBounds;  /// Bounding rectangle of the node.

  QString mLabelText;
  QFont mLabelFont = Fonts::Main;
  bool mPaintLabel = true;
  QRectF mScaledBounds;  /// Scaled bounding rectangle of the node.

  // mutable std::unique_ptr<QSvgRenderer> mShapeSvgRenderer;
  mutable std::unique_ptr<QSvgRenderer> mNodeSvgRenderer;

  qreal computeScaleFactor() const;
};
