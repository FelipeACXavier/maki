#include "node_action_menu.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QSvgWidget>

#include "app_paths.h"
#include "elements/node.h"
#include "system/canvas_view.h"

namespace
{
constexpr int kActionRole = Qt::UserRole;
constexpr int kActionAddFlow = 1;
constexpr int kActionAddSubtask = 2;
constexpr int kGapFromNode = 8;
constexpr int kIconPx = 32;
constexpr int kRowHeight = 36;
constexpr int kIconTextGap = 8;
constexpr int kHPad = 8;
constexpr int kVPad = 4;

void setActionRowHovered(QWidget* row, QTreeWidget* tree, bool hovered)
{
  if (!row || !tree)
    return;

  QPalette p = tree->palette();
  if (hovered)
  {
    p.setColor(QPalette::Window, tree->palette().color(QPalette::Highlight));
    p.setColor(QPalette::WindowText, tree->palette().color(QPalette::HighlightedText));
  }

  row->setAutoFillBackground(hovered);
  row->setPalette(p);
  for (QLabel* label : row->findChildren<QLabel*>())
  {
    label->setAutoFillBackground(hovered);
    label->setPalette(p);
  }
}

class ActionRowHoverFilter final : public QObject
{
public:
  ActionRowHoverFilter(QWidget* row, QTreeWidget* tree)
      : QObject(row), m_row(row), m_tree(tree)
  {
    row->installEventFilter(this);
  }

protected:
  bool eventFilter(QObject* watched, QEvent* event) override
  {
    if (watched != m_row)
      return QObject::eventFilter(watched, event);

    if (event->type() == QEvent::Enter)
      setActionRowHovered(m_row, m_tree, true);
    else if (event->type() == QEvent::Leave)
      setActionRowHovered(m_row, m_tree, false);

    return QObject::eventFilter(watched, event);
  }

private:
  QWidget* m_row = nullptr;
  QTreeWidget* m_tree = nullptr;
};

class NodeActionMenuViewportFilter final : public QObject
{
public:
  explicit NodeActionMenuViewportFilter(QTreeWidget* tree)
      : QObject(tree), m_tree(tree)
  {
  }

protected:
  bool eventFilter(QObject* watched, QEvent* event) override
  {
    if (watched == m_tree->viewport() && event->type() == QEvent::Leave)
    {
      m_tree->setCurrentItem(nullptr);
      for (int i = 0; i < m_tree->topLevelItemCount(); ++i)
      {
        if (auto* row = m_tree->itemWidget(m_tree->topLevelItem(i), 0))
          setActionRowHovered(row, m_tree, false);
      }
    }
    return QObject::eventFilter(watched, event);
  }

private:
  QTreeWidget* m_tree = nullptr;
};

QWidget* makeActionRow(QTreeWidget* tree, const QString& svgPath, const QString& labelText)
{
  auto* row = new QWidget(tree);
  row->setFixedHeight(kRowHeight);
  auto* rowLayout = new QHBoxLayout(row);
  rowLayout->setContentsMargins(kHPad, 0, kHPad, 0);
  rowLayout->setSpacing(kIconTextGap);

  if (!svgPath.isEmpty())
  {
    auto* svg = new QSvgWidget(svgPath, row);
    svg->setFixedSize(kIconPx, kIconPx);
    rowLayout->addWidget(svg, 0, Qt::AlignVCenter);
  }

  auto* label = new QLabel(labelText, row);
  rowLayout->addWidget(label, 0, Qt::AlignVCenter);

  new ActionRowHoverFilter(row, tree);
  return row;
}

void clearAllActionRowHover(QTreeWidget* tree)
{
  if (!tree)
    return;

  tree->setCurrentItem(nullptr);
  for (int i = 0; i < tree->topLevelItemCount(); ++i)
  {
    if (auto* row = tree->itemWidget(tree->topLevelItem(i), 0))
      setActionRowHovered(row, tree, false);
  }
}

void applyCompactTreeSize(QTreeWidget* tree)
{
  if (!tree || tree->topLevelItemCount() == 0)
    return;

  const QFontMetrics fm(tree->font());
  int textW = 0;
  for (int i = 0; i < tree->topLevelItemCount(); ++i)
  {
    if (auto* row = tree->itemWidget(tree->topLevelItem(i), 0))
    {
      for (const QLabel* label : row->findChildren<QLabel*>())
        textW = qMax(textW, fm.horizontalAdvance(label->text()));
    }
  }

  const int treeW = kHPad * 2 + kIconPx + kIconTextGap + textW;
  const int treeH = kVPad * 2 + tree->topLevelItemCount() * kRowHeight;
  tree->setFixedSize(treeW, treeH);
}
}  // namespace

NodeActionMenu::NodeActionMenu(QWidget* parent)
    : QWidget(parent)
{
  setAttribute(Qt::WA_ShowWithoutActivating, true);
  setFocusPolicy(Qt::NoFocus);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  mTree = new QTreeWidget(this);
  mTree->setObjectName(QStringLiteral("NodeActionMenuList"));
  mTree->setHeaderHidden(true);
  mTree->setRootIsDecorated(false);
  mTree->setIndentation(0);
  mTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mTree->setFocusPolicy(Qt::NoFocus);
  mTree->setSelectionMode(QAbstractItemView::NoSelection);
  mTree->setIconSize(QSize(kIconPx, kIconPx));
  mTree->setUniformRowHeights(true);
  mTree->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  mTree->viewport()->installEventFilter(new NodeActionMenuViewportFilter(mTree));

  const QString flowIconPath = AppPaths::icon(QStringLiteral("button_addflow.svg"));
  const QString subtaskIconPath = AppPaths::icon(QStringLiteral("button_addsubtask.svg"));

  auto* flowItem = new QTreeWidgetItem(mTree);
  flowItem->setData(0, kActionRole, kActionAddFlow);
  flowItem->setToolTip(0, QStringLiteral("Add flow"));
  flowItem->setSizeHint(0, QSize(0, kRowHeight));
  mTree->setItemWidget(flowItem, 0, makeActionRow(mTree, flowIconPath, QStringLiteral("Add flow")));

  auto* subtaskItem = new QTreeWidgetItem(mTree);
  subtaskItem->setData(0, kActionRole, kActionAddSubtask);
  subtaskItem->setToolTip(0, QStringLiteral("Add subtask"));
  subtaskItem->setSizeHint(0, QSize(0, kRowHeight));
  mTree->setItemWidget(subtaskItem, 0, makeActionRow(mTree, subtaskIconPath, QStringLiteral("Add subtask")));

  applyCompactTreeSize(mTree);
  layout->addWidget(mTree);
  setFixedSize(mTree->size());

  // itemPressed (not itemClicked) so clicks on setItemWidget rows still activate.
  connect(mTree, &QTreeWidget::itemPressed, this, [this](QTreeWidgetItem* item) {
    if (!item || !mTask)
      return;

    const int action = item->data(0, kActionRole).toInt();
    if (action == kActionAddFlow)
      emit addFlowRequested(mTask);
    else if (action == kActionAddSubtask)
      emit addSubtaskRequested(mTask);

    hideMenu();
  });
}

void NodeActionMenu::hideMenu()
{
  mTask = nullptr;
  hide();
}

QString NodeActionMenu::trackedTaskId() const
{
  return mTask ? mTask->id() : QString();
}

void NodeActionMenu::updatePosition(CanvasView* view)
{
  if (!mTask || !view)
    return;

  const QRectF sceneRect = mTask->mapRectToScene(mTask->boundingRect());
  const QPointF anchorScene(sceneRect.right(), sceneRect.center().y());
  QPoint viewPos = view->mapFromScene(anchorScene);
  viewPos.rx() += kGapFromNode;
  move(viewPos.x(), viewPos.y() - height() / 2);
}

void NodeActionMenu::showForTask(NodeItem* task, CanvasView* view)
{
  if (!task || !view || !view->viewport())
  {
    hideMenu();
    return;
  }

  mTask = task;
  updatePosition(view);
  clearAllActionRowHover(mTree);
  show();
  raise();
}
