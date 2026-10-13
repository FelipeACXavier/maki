#pragma once

#include <QMouseEvent>
#include <QPointer>
#include <QWidget>

class TourGuide;
class QGraphicsItem;
class QGraphicsView;
class QTimer;

class TourOverlay : public QWidget
{
  Q_OBJECT
public:
  TourOverlay(QWidget* topLevel, TourGuide* guide);

  void setSpotlightWidget(QWidget* target);
  void setSpotlightItem(QGraphicsView* view, QGraphicsItem* item);
  void clearSpotlight();

  bool eventFilter(QObject* watched, QEvent* event) override;

  QRect spotlightRectLocal() const;

signals:
  void spotlightMoved(const QRect& localRect);

protected:
  void resizeEvent(QResizeEvent* e) override;
  void paintEvent(QPaintEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseDoubleClickEvent(QMouseEvent*) override;

private:
  QPointer<QWidget> m_target;
  QGraphicsView* m_view = nullptr;
  QGraphicsItem* m_item = nullptr;
  QTimer* m_trackTimer = nullptr;
  TourGuide* m_guide;

  void updateSpotlightRect();
};