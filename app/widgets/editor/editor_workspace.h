#pragma once

#include <QHash>
#include <QIcon>
#include <QMimeData>
#include <QPointer>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QSplitter;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

namespace maki
{
class EditorTabGroup;

constexpr auto MAKI_EDITOR_MIME_TYPE = "application/x-maki-editor-id";

class EditorWorkspace final : public QWidget
{
  Q_OBJECT
public:
  enum class SplitDirection
  {
    Left,
    Right,
    Top,
    Bottom
  };
  Q_ENUM(SplitDirection)

  explicit EditorWorkspace(QWidget* parent = nullptr);
  virtual ~EditorWorkspace();

  QWidget* addEditor(const QString& id, QWidget* editor, const QString& title, const QIcon& icon = {}, bool closable = true);
  QWidget* findEditor(const QString& id) const;
  QString editorId(const QWidget* editor) const;
  QList<QWidget*> editors() const;

  QWidget* activeEditor() const;
  EditorTabGroup* activeGroup() const;

  bool activateEditor(const QString& id);
  bool closeEditor(const QString& id);
  void closeAllExcept(const QString& id);

  bool moveEditor(const QString& id, EditorTabGroup* target, int index = -1);
  bool splitEditor(const QString& id, EditorTabGroup* target, SplitDirection direction);
  bool splitEditor(const QString& id, QWidget* targetEditor, SplitDirection direction);

  void beginTabDrag();
  void endTabDrag();
  bool acceptsDrag(const QMimeData* data) const;
  QString dragId(const QMimeData* data) const;

  void setEditorTitle(const QString& id, const QString& title);
  void setEditorIcon(const QString& id, const QIcon& icon);

signals:
  void activeEditorChanged(QWidget* editor);
  void editorCloseRequested(QWidget* editor);
  void editorClosed(const QString& id);

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  friend class EditorTabGroup;

  struct Entry
  {
    QPointer<QWidget> widget;
    QString title;
    QIcon icon;
    bool closable = true;
  };

  QHash<QString, Entry> mEntries;
  QPointer<EditorTabGroup> mActiveGroup;
  QPointer<QWidget> mActiveEditor;
  QWidget* mRoot = nullptr;
  QVBoxLayout* mLayout = nullptr;
  bool mTransferring = false;

  EditorTabGroup* createGroup();
  EditorTabGroup* groupFor(QWidget* editor) const;

  void setActiveGroup(EditorTabGroup* group);
  void removeEmptyGroup(EditorTabGroup* group);

  void normalize(QSplitter* splitter);
  bool transfer(QWidget* editor, EditorTabGroup* target, int index);

  int groupCount() const;
};

}  // namespace maki
