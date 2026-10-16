#include "editor_tab_group.h"

#include "app_configs.h"
#include "editor_drop_overlay.h"
#include "editor_focus_border.h"
#include "editor_tab_bar.h"
#include "editor_workspace.h"

namespace maki
{
EditorTabGroup::EditorTabGroup(EditorWorkspace* workspace)
    : QTabWidget(workspace)
    , mWorkspace(workspace)
{
  setTabBar(new EditorTabBar(this));
  mOverlay = new EditorDropOverlay(this);
  mFocusBorder = new EditorFocusBorder(this);

  setTabsClosable(true);
  setDocumentMode(true);
  setAcceptDrops(true);
  setMinimumSize(160, 110);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Tab bar stuff
  tabBar()->setExpanding(false);
  tabBar()->setIconSize(Config::SMALL_BUTTON_SIZE);
  tabBar()->setUsesScrollButtons(false);
  tabBar()->setChangeCurrentOnDrag(false);

  connect(tabBar(), &QTabBar::tabMoved, this, [this](int /* from */, int /* to */) {
    for (int i = 0; i < count(); ++i)
      setTabText(i, tabText(i));
  });
}

void EditorTabGroup::setActive(bool active)
{
  if (!mFocusBorder)
    return;

  mFocusBorder->setActive(active);
  updateFocusBorderGeometry();
}

void EditorTabGroup::resizeEvent(QResizeEvent* event)
{
  QTabWidget::resizeEvent(event);

  // Keep the tab bar usable; cover only the page/editor content.
  const int top = tabBar()->geometry().bottom() + 1;
  mOverlay->setGeometry(0, top, width(), std::max(0, height() - top));

  updateFocusBorderGeometry();
}

void EditorTabGroup::updateFocusBorderGeometry()
{
  if (!mFocusBorder)
    return;

  if (auto* editor = currentWidget(); editor)
  {
    // Convert editor-local coordinates to EditorTabGroup coordinates.
    const QPoint topLeft = editor->mapTo(this, QPoint(0, 0));
    mFocusBorder->setGeometry(topLeft.x(), topLeft.y(), editor->width(), 2);
    if (mFocusBorder->isVisible())
      mFocusBorder->raise();
  }
  else
  {
    mFocusBorder->setActive(false);
  }
}
}  // namespace maki