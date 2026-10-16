#include "editor_workspace.h"

#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QSplitter>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>

#include "editor_drop_overlay.h"
#include "editor_tab_group.h"
#include "macros.h"

namespace maki
{
EditorWorkspace::EditorWorkspace(QWidget* parent)
    : QWidget(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mLayout = new QVBoxLayout(this);
  mLayout->setContentsMargins(0, 0, 0, 0);
  mLayout->setSpacing(0);

  mRoot = createGroup();
  mLayout->addWidget(mRoot);

  setActiveGroup(qobject_cast<EditorTabGroup*>(mRoot));
  qApp->installEventFilter(this);
}

EditorWorkspace::~EditorWorkspace()
{
  qApp->removeEventFilter(this);
}

EditorTabGroup* EditorWorkspace::createGroup()
{
  auto* group = new EditorTabGroup(this);
  group->installEventFilter(this);
  group->tabBar()->installEventFilter(this);
  connect(group, &QTabWidget::currentChanged, this, [this, group](int) {
    if (!mTransferring)
      setActiveGroup(group);
  });
  connect(group, &QTabWidget::tabCloseRequested, this, [this, group](int index) {
    if (QWidget* widget = group->widget(index))
    {
      const auto id = editorId(widget);
      if (!id.isEmpty() && mEntries.value(id).closable)
        emit editorCloseRequested(widget);
    }
  });
  return group;
}

QString EditorWorkspace::editorId(const QWidget* editor) const
{
  for (auto it = mEntries.cbegin(); it != mEntries.cend(); ++it)
    if (it.value().widget == editor)
      return it.key();

  return {};
}

QWidget* EditorWorkspace::findEditor(const QString& id) const
{
  return mEntries.value(id).widget;
}

QList<QWidget*> EditorWorkspace::editors() const
{
  QList<QWidget*> result;
  for (const auto& entry : mEntries)
    if (entry.widget)
      result.append(entry.widget);
  return result;
}

EditorTabGroup* EditorWorkspace::groupFor(QWidget* editor) const
{
  if (!editor)
    return nullptr;

  auto group = maki::findIf(findChildren<EditorTabGroup*>(), [editor](const auto& opt) { return opt->indexOf(editor) >= 0; });
  if (group)
    return *group;

  return nullptr;
}

QWidget* EditorWorkspace::activeEditor() const
{
  return mActiveEditor;
}

EditorTabGroup* EditorWorkspace::activeGroup() const
{
  return mActiveGroup;
}

void EditorWorkspace::setActiveGroup(EditorTabGroup* group)
{
  // Even if the group didn't change, the editor might have changed
  if (mActiveGroup != group)
  {
    if (auto* active = activeGroup())
      active->setActive(false);

    mActiveGroup = group;

    if (auto* active = activeGroup(); groupCount() > 1)
      active->setActive(true);
  }

  if (auto* active = activeGroup(); active->currentWidget() != activeEditor())
  {
    mActiveEditor = active->currentWidget();
    emit activeEditorChanged(activeEditor());
  }
}

QWidget* EditorWorkspace::addEditor(const QString& id, QWidget* editor, const QString& title, const QIcon& icon, bool closable)
{
  if (id.isEmpty() || !editor)
    return nullptr;

  if (QWidget* existing = findEditor(id))
  {
    activateEditor(id);
    return existing;
  }

  EditorTabGroup* group = mActiveGroup ? mActiveGroup.data() : qobject_cast<EditorTabGroup*>(mRoot);
  if (!group)
    return nullptr;

  mEntries.insert(id, {editor, title, icon, closable});
  const int index = group->addTab(editor, icon, title);
  if (!closable)
    group->tabBar()->setTabButton(index, QTabBar::RightSide, nullptr);

  group->setCurrentIndex(index);
  setActiveGroup(group);

  return editor;
}

bool EditorWorkspace::activateEditor(const QString& id)
{
  QWidget* editor = findEditor(id);
  EditorTabGroup* group = groupFor(editor);
  if (!group)
    return false;

  group->setCurrentWidget(editor);
  setActiveGroup(group);

  return true;
}

bool EditorWorkspace::closeEditor(const QString& id)
{
  QWidget* editor = findEditor(id);
  EditorTabGroup* group = groupFor(editor);
  if (!group)
    return false;

  const int index = group->indexOf(editor);
  if (index < 0)
    return false;

  const bool wasActive = (mActiveEditor == editor);

  mTransferring = true;
  group->removeTab(index);
  editor->setParent(this);
  mEntries.remove(id);
  mTransferring = false;

  if (wasActive)
    mActiveEditor = nullptr;

  if (group->count() == 0)
    removeEmptyGroup(group);

  if (mActiveGroup)
    setActiveGroup(mActiveGroup);

  emit editorClosed(id);
  editor->deleteLater();
  return true;
}

void EditorWorkspace::closeAllExcept(const QString& id)
{
  const auto ids = mEntries.keys();
  for (const QString& candidate : ids)
    if (candidate != id)
      closeEditor(candidate);
}

bool EditorWorkspace::transfer(QWidget* editor, EditorTabGroup* target, int index)
{
  auto* source = groupFor(editor);
  if (!source || !target)
    return false;

  const QString id = editorId(editor);
  if (id.isEmpty())
    return false;

  const Entry entry = mEntries.value(id);
  if (source == target && index < 0)
  {
    activateEditor(id);
    return true;
  }

  mTransferring = true;
  source->removeTab(source->indexOf(editor));
  editor->setParent(this);

  const int insertAt = index < 0 ? target->count() : std::clamp(index, 0, target->count());
  const int inserted = target->insertTab(insertAt, editor, entry.icon, entry.title);
  if (!entry.closable)
    target->tabBar()->setTabButton(inserted, QTabBar::RightSide, nullptr);

  target->setCurrentIndex(inserted);
  mTransferring = false;
  setActiveGroup(target);

  if (source != target && source->count() == 0)
    removeEmptyGroup(source);

  QMetaObject::invokeMethod(target, [target]() { target->updateFocusBorderGeometry(); }, Qt::QueuedConnection);
  return true;
}

bool EditorWorkspace::moveEditor(const QString& id, EditorTabGroup* target, int index)
{
  return transfer(findEditor(id), target, index);
}

bool EditorWorkspace::splitEditor(const QString& id, QWidget* targetEditor, SplitDirection direction)
{
  return splitEditor(id, groupFor(targetEditor), direction);
}

bool EditorWorkspace::splitEditor(const QString& id, EditorTabGroup* target, SplitDirection direction)
{
  QWidget* editor = findEditor(id);
  if (!editor || !target)
    return false;

  const auto* groupOfEditor = groupFor(editor);
  if (groupOfEditor == target && target->count() == 1)
    return false;

  const bool before = direction == SplitDirection::Left || direction == SplitDirection::Top;
  const Qt::Orientation orientation = (direction == SplitDirection::Left || direction == SplitDirection::Right) ? Qt::Horizontal : Qt::Vertical;
  auto* group = createGroup();
  auto* parent = qobject_cast<QSplitter*>(target->parentWidget());
  if (parent && parent->orientation() == orientation)
  {
    const int index = parent->indexOf(target);
    parent->insertWidget(index + (before ? 0 : 1), group);
  }
  else
  {
    auto* splitter = new QSplitter(orientation);
    splitter->setChildrenCollapsible(false);
    if (parent)
    {
      const int index = parent->indexOf(target);
      // replaceWidget detaches target. Reattach it under the new splitter.
      parent->replaceWidget(index, splitter);
    }
    else
    {
      mLayout->replaceWidget(target, splitter);
      mRoot = splitter;
    }

    if (before)
    {
      splitter->addWidget(group);
      splitter->addWidget(target);
    }
    else
    {
      splitter->addWidget(target);
      splitter->addWidget(group);
    }
    splitter->setSizes({500, 500});
  }

  return transfer(editor, group, -1);
}

void EditorWorkspace::beginTabDrag()
{
  for (auto* group : findChildren<EditorTabGroup*>())
    if (group->isVisibleTo(this))
      group->dropOverlay()->beginDrag();
}

void EditorWorkspace::endTabDrag()
{
  for (auto* group : findChildren<EditorTabGroup*>())
    group->dropOverlay()->endDrag();
}

void EditorWorkspace::removeEmptyGroup(EditorTabGroup* group)
{
  if (!group || group->count() != 0)
    return;

  auto* parent = qobject_cast<QSplitter*>(group->parentWidget());
  if (!parent)
    return;  // Keep one empty group for new editors.

  if (mActiveGroup == group)
    mActiveGroup = nullptr;

  group->hide();
  group->setParent(this);
  group->deleteLater();
  normalize(parent);
  if (!mActiveGroup)
  {
    const auto groups = findChildren<EditorTabGroup*>();
    for (auto* candidate : groups)
      if (candidate != group && candidate->isVisibleTo(this))
      {
        setActiveGroup(candidate);
        break;
      }
  }
}

void EditorWorkspace::normalize(QSplitter* splitter)
{
  while (splitter && splitter->count() == 1)
  {
    QWidget* child = splitter->widget(0);
    auto* parent = qobject_cast<QSplitter*>(splitter->parentWidget());
    child->setParent(this);
    if (parent)
      parent->replaceWidget(parent->indexOf(splitter), child);
    else
    {
      mLayout->replaceWidget(splitter, child);
      mRoot = child;
    }
    splitter->hide();
    splitter->deleteLater();
    splitter = parent;
  }
}

void EditorWorkspace::setEditorTitle(const QString& id, const QString& title)
{
  if (!mEntries.contains(id))
    return;
  mEntries[id].title = title;
  QWidget* editor = findEditor(id);
  if (auto* group = groupFor(editor))
    group->setTabText(group->indexOf(editor), title);
}

void EditorWorkspace::setEditorIcon(const QString& id, const QIcon& icon)
{
  if (!mEntries.contains(id))
    return;

  mEntries[id].icon = icon;
  QWidget* editor = findEditor(id);
  if (auto* group = groupFor(editor))
    group->setTabIcon(group->indexOf(editor), icon);
}

bool EditorWorkspace::acceptsDrag(const QMimeData* data) const
{
  return !dragId(data).isEmpty();
}

QString EditorWorkspace::dragId(const QMimeData* data) const
{
  if (!data || !data->hasFormat(MAKI_EDITOR_MIME_TYPE))
    return {};
  const QString id = QString::fromUtf8(data->data(MAKI_EDITOR_MIME_TYPE));
  return findEditor(id) ? id : QString{};
}

bool EditorWorkspace::eventFilter(QObject* watched, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)
  {
    for (QWidget* widget = qobject_cast<QWidget*>(watched); widget; widget = widget->parentWidget())
      if (auto* group = qobject_cast<EditorTabGroup*>(widget))
      {
        setActiveGroup(group);
        break;
      }
  }

  return QWidget::eventFilter(watched, event);
}

int EditorWorkspace::groupCount() const
{
  return findChildren<QSplitter*>().count() + 1;
}

}  // namespace maki
