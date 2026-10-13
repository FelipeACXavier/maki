#include "tour_overlay.h"

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRectF>
#include <QTimer>
#include <oclero/qlementine.hpp>

#include "tour_guide.h"

TourOverlay::TourOverlay(QWidget* topLevel, TourGuide* guide)
    : QWidget(topLevel)
    , m_guide(guide)
{
  raise();
  hide();

  // Items move through the scene, not via widget events, so while a
  // QGraphicsItem is spotlighted we poll at ~30 Hz to follow it.
  m_trackTimer = new QTimer(this);
  m_trackTimer->setInterval(33);
  connect(m_trackTimer, &QTimer::timeout, this, [this] {
    updateSpotlightRect();
    update();
  });
}

void TourOverlay::setSpotlightWidget(QWidget* target)
{
  m_trackTimer->stop();
  if (m_target)
    m_target->removeEventFilter(this);

  m_target = target;
  m_view = nullptr;
  m_item = nullptr;
  if (target)
    target->installEventFilter(this);

  updateSpotlightRect();
  update();
}

void TourOverlay::setSpotlightItem(QGraphicsView* view, QGraphicsItem* item)
{
  m_trackTimer->stop();
  if (m_target)
    m_target->removeEventFilter(this);

  m_target = nullptr;
  m_view = view;
  m_item = item;
  if (view && item)
    m_trackTimer->start();

  updateSpotlightRect();
  update();
}

void TourOverlay::clearSpotlight()
{
  m_trackTimer->stop();
  if (m_target)
    m_target->removeEventFilter(this);
  m_target = nullptr;
  m_view = nullptr;
  m_item = nullptr;
  update();
}

bool TourOverlay::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == m_target)
  {
    switch (event->type())
    {
      case QEvent::Move:
      case QEvent::Resize:
      case QEvent::Show:
      case QEvent::Hide:
      case QEvent::Paint:
        updateSpotlightRect();
        update();
        break;
      default:
        break;
    }
  }
  return QWidget::eventFilter(watched, event);
}

QRect TourOverlay::spotlightRectLocal() const
{
  constexpr int padding = 8;

  if (m_target && m_target->isVisible())
  {
    // Find whether the target is embedded in a QGraphicsProxyWidget.
    QGraphicsProxyWidget* proxy = nullptr;

    for (QWidget* widget = m_target; widget; widget = widget->parentWidget())
    {
      if (auto* candidate = widget->graphicsProxyWidget())
      {
        proxy = candidate;
        break;
      }
    }

    if (proxy && proxy->scene())
    {
      // Find the view displaying the proxy.
      QGraphicsView* view = nullptr;

      for (auto* candidate : proxy->scene()->views())
      {
        if (candidate->isVisible())
        {
          view = candidate;
          break;
        }
      }

      if (view)
      {
        // Widget -> Proxy -> Scene -> View -> Overlay
        const QRectF proxyRect = proxy->subWidgetRect(m_target);
        const QRectF sceneRect = proxy->mapToScene(proxyRect).boundingRect();
        const QRect viewRect = view->mapFromScene(sceneRect).boundingRect();
        const QPoint topLeft = mapFromGlobal(view->viewport()->mapToGlobal(viewRect.topLeft()));
        return QRect(topLeft, viewRect.size()).adjusted(-padding, -padding, padding, padding);
      }
    }

    // Original implementation for ordinary widgets.
    return QRect(m_target->mapTo(parentWidget(), QPoint(0, 0)), m_target->size()).adjusted(-padding, -padding, padding, padding);
  }

  // Original implementation for QGraphicsItem targets.
  if (m_view && m_item && m_view->isVisible() && m_item->scene())
  {
    const QRectF sceneRect = m_item->mapToScene(m_item->boundingRect()).boundingRect();
    const QRect viewRect = m_view->mapFromScene(sceneRect).boundingRect();
    return QRect(m_view->mapTo(parentWidget(), viewRect.topLeft()), viewRect.size()).adjusted(-padding, -padding, padding, padding);
  }

  return {};
}

void TourOverlay::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
  updateSpotlightRect();
  update();
  emit spotlightMoved(spotlightRectLocal());
}

void TourOverlay::paintEvent(QPaintEvent*)
{
  auto* appStyle = oclero::qlementine::appStyle();
  if (!appStyle)
    return;

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const auto theme = appStyle->theme();
  const QRect hole = spotlightRectLocal();
  if (!hole.isNull())
  {
    QRegion windowRegion(rect());
    QRegion holeRegion(hole.adjusted(2, 2, -2, -2), QRegion::Ellipse);
    setMask(windowRegion.subtracted(holeRegion));

    QPainterPath outside;
    outside.addRect(rect());
    outside.addRoundedRect(hole, theme.borderRadius, theme.borderRadius);
    p.fillPath(outside, theme.shadowColor3);
    p.setPen(QPen(theme.primaryColor, theme.borderWidth));
    p.drawRoundedRect(hole, theme.borderRadius, theme.borderRadius);
  }
  else
  {
    clearMask();
    p.fillRect(rect(), theme.shadowColor3);
  }
}

void TourOverlay::mousePressEvent(QMouseEvent*)
{
}

void TourOverlay::mouseDoubleClickEvent(QMouseEvent*)
{
}

void TourOverlay::updateSpotlightRect()
{
  if (m_target || m_item)
    emit spotlightMoved(spotlightRectLocal());
}