#include "editor_drop_overlay.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPainter>
#include <algorithm>

#include "editor_tab_group.h"
#include "editor_workspace.h"

namespace maki
{
EditorDropOverlay::EditorDropOverlay(EditorTabGroup* group)
    : QWidget(group)
    , mGroup(group)
{
  setAcceptDrops(true);
  setAttribute(Qt::WA_NoSystemBackground);
  hide();
}

void EditorDropOverlay::beginDrag()
{
  mZone = Zone::None;
  show();
  raise();
  update();
}

void EditorDropOverlay::endDrag()
{
  mZone = Zone::None;
  hide();
}

EditorDropOverlay::Zone EditorDropOverlay::zoneAt(QPoint point) const
{
  const QRect r = rect();
  if (!r.contains(point))
    return Zone::None;
  // The edges are deliberately generous, but never exceed one third.
  const int edgeX = std::min(r.width() / 3, std::max(36, r.width() / 4));
  const int edgeY = std::min(r.height() / 3, std::max(36, r.height() / 4));
  if (point.x() < edgeX)
    return Zone::Left;
  if (point.x() >= r.width() - edgeX)
    return Zone::Right;
  if (point.y() < edgeY)
    return Zone::Top;
  if (point.y() >= r.height() - edgeY)
    return Zone::Bottom;
  return Zone::Centre;
}

QRect EditorDropOverlay::previewRect(Zone zone) const
{
  QRect r = rect().adjusted(4, 4, -5, -5);
  switch (zone)
  {
    case Zone::Left:
      r.setWidth(r.width() / 2);
      break;
    case Zone::Right:
      r.setLeft(r.center().x() + 1);
      break;
    case Zone::Top:
      r.setHeight(r.height() / 2);
      break;
    case Zone::Bottom:
      r.setTop(r.center().y() + 1);
      break;
    default:
      break;
  }
  return r;
}

void EditorDropOverlay::dragEnterEvent(QDragEnterEvent* event)
{
  if (mGroup->workspace()->acceptsDrag(event->mimeData()))
    event->acceptProposedAction();
  else
    event->ignore();
}

void EditorDropOverlay::dragMoveEvent(QDragMoveEvent* event)
{
  if (!mGroup->workspace()->acceptsDrag(event->mimeData()))
  {
    event->ignore();
    return;
  }
  mZone = zoneAt(event->position().toPoint());
  update();
  event->acceptProposedAction();
}

void EditorDropOverlay::dragLeaveEvent(QDragLeaveEvent* event)
{
  mZone = Zone::None;
  update();
  event->accept();
}

void EditorDropOverlay::dropEvent(QDropEvent* event)
{
  auto* workspace = mGroup->workspace();
  const QString id = workspace->dragId(event->mimeData());
  const Zone zone = zoneAt(event->position().toPoint());
  mZone = Zone::None;
  if (id.isEmpty())
  {
    event->ignore();
    return;
  }
  // Hide overlays BEFORE changing the splitter tree; this may destroy groups.
  workspace->endTabDrag();
  bool ok = false;
  switch (zone)
  {
    case Zone::Centre:
      ok = workspace->moveEditor(id, mGroup);
      break;
    case Zone::Left:
      ok = workspace->splitEditor(id, mGroup, EditorWorkspace::SplitDirection::Left);
      break;
    case Zone::Right:
      ok = workspace->splitEditor(id, mGroup, EditorWorkspace::SplitDirection::Right);
      break;
    case Zone::Top:
      ok = workspace->splitEditor(id, mGroup, EditorWorkspace::SplitDirection::Top);
      break;
    case Zone::Bottom:
      ok = workspace->splitEditor(id, mGroup, EditorWorkspace::SplitDirection::Bottom);
      break;
    default:
      break;
  }
  if (ok)
    event->acceptProposedAction();
  else
    event->ignore();
}

void EditorDropOverlay::paintEvent(QPaintEvent*)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  const QColor accent = palette().highlight().color();
  if (mZone == Zone::None)
  {
    // A subtle guide makes it obvious that the editor area is droppable.
    QColor guide = accent;
    guide.setAlpha(28);
    p.fillRect(rect(), guide);
    return;
  }
  QColor fill = accent;
  fill.setAlpha(85);
  QColor border = accent;
  border.setAlpha(220);
  const QRect r = previewRect(mZone);
  p.setBrush(fill);
  p.setPen(QPen(border, 2));
  p.drawRoundedRect(r.adjusted(1, 1, -1, -1), 5, 5);
}
}  // namespace maki
