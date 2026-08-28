#include "task_node_menu.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QFontMetrics>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QSvgWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>

#include "app_configs.h"
#include "style_helpers.h"

NodeActionRow::NodeActionRow(const QString& svgPath, const QString& labelText, QWidget* parent)
    : StyledFrame(parent)
    , mHovered(false)
{
  auto* style = oclero::qlementine::appStyle();
  if (!style)
    return;

  auto theme = style->theme();

  setMouseTracking(true);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::NoFocus);

  setBackgroundRole(StyledFrame::BackgroundRole::Base);
  setBorderWidth(0);

  setAttribute(Qt::WA_Hover, true);
  setFixedHeight(theme.controlHeightLarge);

  auto* rowLayout = new QHBoxLayout(this);
  // Leave some space on the left and right so the hover goes over the icon and text
  rowLayout->setContentsMargins(theme.tabBarPaddingTop, 0, theme.tabBarPaddingTop, 0);
  rowLayout->setSpacing(theme.spacing);

  if (!svgPath.isEmpty())
  {
    auto* svg = new QSvgWidget(svgPath, this);
    svg->setFixedSize(Config::SMALL_BUTTON_SIZE);
    rowLayout->addWidget(svg, 0, Qt::AlignVCenter);
  }

  auto* label = new QLabel(labelText, this);
  rowLayout->addWidget(label, 2, Qt::AlignVCenter);
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
  if (mHovered == hovered)
    return;

  mHovered = hovered;
  update();
}

void NodeActionRow::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  if (const auto* qlementineStyle = oclero::qlementine::appStyle(); mHovered)
  {
    painter.setPen(Qt::NoPen);
    painter.setBrush(qlementineStyle->theme().neutralColorHovered);
    painter.drawRoundedRect(rect(), qlementineStyle->theme().borderRadius, qlementineStyle->theme().borderRadius);
  }

  QWidget::paintEvent(event);
}

// ============================================================================
// TaskNodeMenu
// ============================================================================
TaskNodeMenu* TaskNodeMenu::create(QWidget* parent)
{
  auto* menu = new TaskNodeMenu(parent);
  return menu;
}

TaskNodeMenu::TaskNodeMenu(QWidget* parent)
    : maki::ControlWidget(parent)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();

  // Since this is an overlay, keep it above the normal canvas widgets.
  raise();

  auto frame = new StyledFrame(this);
  frame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  frame->setBorderWidth(theme.borderWidth);
  frame->setRadius(theme.borderRadius);
  frame->setCustomBorderColor(theme.primaryColor);

  auto* mainLayout = new QVBoxLayout(frame);
  mainLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  mainLayout->setSpacing(0);

  const QString mainIconPath = iconPathFromTheme("icon_start");
  const QString flowIconPath = iconPathFromTheme("button_addflow");
  const QString subtaskIconPath = iconPathFromTheme("button_addsubtask");

  auto* mainWidget = new NodeActionRow(mainIconPath, tr("Open main flow"), this);
  auto* flowWidget = new NodeActionRow(flowIconPath, tr("Add flow"), this);
  auto* subtaskWidget = new NodeActionRow(subtaskIconPath, tr("Add subtask"), this);
  mainLayout->addWidget(mainWidget);
  mainLayout->addWidget(flowWidget);
  mainLayout->addWidget(subtaskWidget);

  connect(mainWidget, &NodeActionRow::clicked, this, [this] {
    emit openMainFlowRequested();
    hideMenu();
  });
  connect(flowWidget, &NodeActionRow::clicked, this, [this] {
    emit addFlowRequested();
    hideMenu();
  });
  connect(subtaskWidget, &NodeActionRow::clicked, this, [this] {
    emit addSubtaskRequested();
    hideMenu();
  });

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(frame);
}

void TaskNodeMenu::hideMenu()
{
  LOG_DEBUG("Sending dismissed");
  emit dismissed();
}
