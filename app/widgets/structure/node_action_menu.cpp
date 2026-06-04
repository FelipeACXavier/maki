#include "node_action_menu.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QFontMetrics>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSvgWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "elements/node.h"
#include "style_helpers.h"
#include "system/canvas_view.h"

namespace
{
constexpr int kGapFromNode = 8;
constexpr int kIconPx = 32;
constexpr int kRowHeight = 36;
constexpr int kIconTextGap = 8;
constexpr int kHPad = 8;
}  // namespace

NodeActionRow::NodeActionRow(const QString& svgPath, const QString& labelText, QWidget* parent)
    : QWidget(parent)
    , mHovered(false)
{
  setMouseTracking(true);
  setAttribute(Qt::WA_Hover, true);
  setFixedHeight(kRowHeight);

  auto* rowLayout = new QHBoxLayout(this);
  rowLayout->setContentsMargins(kHPad, 0, kHPad, 0);
  rowLayout->setSpacing(kIconTextGap);

  if (!svgPath.isEmpty())
  {
    auto* svg = new QSvgWidget(svgPath, this);
    svg->setFixedSize(kIconPx, kIconPx);
    rowLayout->addWidget(svg, 0, Qt::AlignVCenter);
  }

  auto* label = new QLabel(labelText, this);
  rowLayout->addWidget(label, 0, Qt::AlignVCenter);
}

void NodeActionRow::enterEvent(QEnterEvent* event)
{
  setHovered(true);
  QWidget::enterEvent(event);
}

void NodeActionRow::leaveEvent(QEvent* event)
{
  setHovered(false);
  QWidget::leaveEvent(event);
}

void NodeActionRow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
    emit clicked();

  QWidget::mousePressEvent(event);
}

void NodeActionRow::setHovered(bool hovered)
{
  mHovered = hovered;
}

void NodeActionRow::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  if (const auto* qlementineStyle = oclero::qlementine::appStyle())
  {
    painter.setPen(Qt::NoPen);
    painter.setBrush(mHovered ? qlementineStyle->theme().neutralColorHovered : qlementineStyle->theme().backgroundColorMain1);
    painter.drawRect(rect());
  }

  QWidget::paintEvent(event);
}

NodeActionMenu::NodeActionMenu(QWidget* parent)
    : QWidget(parent)
{
  setAttribute(Qt::WA_ShowWithoutActivating, true);
  setFocusPolicy(Qt::NoFocus);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  const QString flowIconPath = iconPathFromTheme("button_addflow.svg");
  const QString subtaskIconPath = iconPathFromTheme("button_addsubtask.svg");

  auto* flowWidget = new NodeActionRow(flowIconPath, QStringLiteral("Add flow"), this);
  auto* subtaskWidget = new NodeActionRow(subtaskIconPath, QStringLiteral("Add subtask"), this);
  layout->addWidget(flowWidget);
  layout->addWidget(subtaskWidget);

  connect(flowWidget, &NodeActionRow::clicked, this, [this] {
    if (mTask)
      emit addFlowRequested(mTask);

    hideMenu();
  });
  connect(subtaskWidget, &NodeActionRow::clicked, this, [this] {
    if (mTask)
      emit addSubtaskRequested(mTask);

    hideMenu();
  });

  mFadeTimer = new QTimer(this);
  mFadeTimer->setSingleShot(true);
  mFadeTimer->setInterval(2000);
  connect(mFadeTimer, &QTimer::timeout, this, &NodeActionMenu::hideMenu);
}

void NodeActionMenu::hideMenu()
{
  mTask = nullptr;
  hide();
}

void NodeActionMenu::enterEvent(QEnterEvent* event)
{
  mFadeTimer->stop();
  QWidget::enterEvent(event);
}

void NodeActionMenu::leaveEvent(QEvent* event)
{
  QWidget::leaveEvent(event);
  mFadeTimer->start();
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
  show();
  raise();
  mFadeTimer->start();
}
