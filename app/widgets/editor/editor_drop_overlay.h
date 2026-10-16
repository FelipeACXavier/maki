#pragma once

#include <QWidget>

class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QPaintEvent;

namespace maki
{
class EditorTabGroup;

// A transparent-on-idle, full-editor-area drag target. It is made visible
// before QDrag::exec(), so the canvas cannot intercept the drag.
class EditorDropOverlay final : public QWidget
{
  Q_OBJECT
public:
  enum class Zone
  {
    None,
    Centre,
    Left,
    Right,
    Top,
    Bottom
  };

  explicit EditorDropOverlay(EditorTabGroup* group);
  void beginDrag();
  void endDrag();

protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

private:
  EditorTabGroup* mGroup;
  Zone mZone = Zone::None;

  Zone zoneAt(QPoint point) const;
  QRect previewRect(Zone zone) const;
};
}  // namespace maki
