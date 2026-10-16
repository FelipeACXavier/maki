#include "editor_tab_bar.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <oclero/qlementine.hpp>

#include "app_configs.h"
#include "editor_tab_group.h"
#include "editor_workspace.h"
#include "style_helpers.h"

namespace maki
{
EditorTabBar::EditorTabBar(QWidget* parent)
    : QTabBar(parent)
{
  // QDrag handles cross-group movement
  setMovable(false);
  setAcceptDrops(false);
}

void EditorTabBar::mousePressEvent(QMouseEvent* event)
{
  mPressPosition = event->pos();
  mPressIndex = tabAt(event->pos());
  QTabBar::mousePressEvent(event);
}

void EditorTabBar::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton) || mPressIndex < 0 || (event->pos() - mPressPosition).manhattanLength() < QApplication::startDragDistance())
  {
    QTabBar::mouseMoveEvent(event);
    return;
  }

  auto* group = qobject_cast<EditorTabGroup*>(parentWidget());
  if (!group)
  {
    QTabBar::mouseMoveEvent(event);
    return;
  }

  auto* workspace = group->workspace();
  QWidget* editor = group->widget(mPressIndex);
  if (!editor)
    return;

  const QString id = workspace->editorId(editor);
  if (id.isEmpty())
    return;

  const QRect tabGeometry = tabRect(mPressIndex);

  // Render the actual tab with its Qlementine styling.
  const qreal dpr = devicePixelRatioF();
  QPixmap pixmap((tabGeometry.size() * dpr));
  pixmap.setDevicePixelRatio(dpr);
  pixmap.fill(Qt::transparent);

  auto* mime = new QMimeData;
  mime->setData(MAKI_EDITOR_MIME_TYPE, id.toUtf8());

  auto* drag = new QDrag(this);
  drag->setMimeData(mime);

  const QPixmap preview = createDragPreview(mPressIndex);
  if (!preview.isNull())
  {
    drag->setPixmap(preview);
    // Position at the center top of the mouse so the user can actually see the icon and title
    drag->setHotSpot(QPoint(preview.width() / (2 * preview.devicePixelRatioF()), 0));
  }
  mPressIndex = -1;

  workspace->beginTabDrag();
  drag->exec(Qt::MoveAction);
  workspace->endTabDrag();
}

QPixmap EditorTabBar::createDragPreview(int index) const
{
  auto* group = qobject_cast<EditorTabGroup*>(parentWidget());
  if (!group || index < 0 || index >= group->count())
    return {};

  QWidget* editor = group->widget(index);
  if (!editor)
    return {};

  const auto* appStyle = oclero::qlementine::appStyle();
  if (!appStyle)
    return {};

  const auto theme = appStyle->theme();
  constexpr int previewWidth = 260;
  constexpr int previewHeight = 180;
  constexpr int headerHeight = 24;

  const qreal dpr = devicePixelRatioF();
  QPixmap preview(QSize(qRound(previewWidth * dpr), qRound(previewHeight * dpr)));
  preview.setDevicePixelRatio(dpr);
  preview.fill(Qt::transparent);

  QPainter painter(&preview);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  const QColor background = theme.backgroundColorMain1;
  const QColor foreground = theme.secondaryColor;
  const QColor border = theme.borderColor;

  // Rounded background.
  painter.setPen(border);
  painter.setBrush(background);
  painter.drawRoundedRect(QRectF(0.5, 0.5, previewWidth - 1, previewHeight - 1), theme.borderRadius, theme.borderRadius);

  // Editor icon.
  const QIcon icon = tabIcon(index);
  if (!icon.isNull())
    painter.drawPixmap(theme.spacing, (headerHeight - Config::SMALL_BUTTON) / 2, applyColorToIcon(icon.pixmap(Config::SMALL_BUTTON_SIZE), foreground));

  // Editor title.
  // We need to account for the space around the icon as well
  const int textX = icon.isNull() ? theme.spacing : theme.spacing + (Config::SMALL_BUTTON + theme.spacing);
  const QRect titleRect(textX, 0, previewWidth - textX - theme.spacing, headerHeight);

  const QString title = fontMetrics().elidedText(tabText(index), Qt::ElideRight, titleRect.width());
  painter.setPen(foreground);
  painter.drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeft, title);

  // Capture the editor's visible content.
  const QPixmap screenshot = editor->grab();
  if (!screenshot.isNull())
  {
    const QRect contentRect(theme.spacing, headerHeight + 2, previewWidth - 2 * theme.spacing, previewHeight - headerHeight - theme.spacing - 2);
    const QPixmap thumbnail = screenshot.scaled(contentRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const QPoint pos(contentRect.x() + (contentRect.width() - thumbnail.width() / thumbnail.devicePixelRatioF()) / 2,
                     contentRect.y() + (contentRect.height() - thumbnail.height() / thumbnail.devicePixelRatioF()) / 2);

    painter.save();
    painter.setClipRect(contentRect);
    painter.drawPixmap(pos, thumbnail);
    painter.restore();
  }

  painter.end();

  return preview;
}
}  // namespace maki