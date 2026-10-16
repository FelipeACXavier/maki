#pragma once

#include <QTabWidget>

namespace maki
{

class EditorWorkspace;
class EditorDropOverlay;
class EditorFocusBorder;

class EditorTabGroup final : public QTabWidget
{
  Q_OBJECT
public:
  explicit EditorTabGroup(EditorWorkspace* workspace);

  EditorWorkspace* workspace() const
  {
    return mWorkspace;
  }

  EditorDropOverlay* dropOverlay() const
  {
    return mOverlay;
  }

  void setActive(bool active);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  friend class EditorWorkspace;
  EditorWorkspace* mWorkspace;
  EditorDropOverlay* mOverlay = nullptr;
  EditorFocusBorder* mFocusBorder = nullptr;

  void updateFocusBorderGeometry();
};
}  // namespace maki