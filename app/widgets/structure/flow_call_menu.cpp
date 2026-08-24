#include "flow_call_menu.h"

#include <QComboBox>
#include <QDialog>
#include <QFrame>
#include <QHideEvent>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QSizePolicy>
#include <QSvgRenderer>
#include <QTimer>
#include <QVBoxLayout>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "app_configs.h"
#include "elements/behaviour/flow_call_node.h"
#include "elements/node.h"
#include "event_dialog.h"
#include "flow_info.h"
#include "keys.h"
#include "node_info.h"
#include "save_info.h"
#include "style_helpers.h"
#include "system/canvas_view.h"
#include "types.h"

namespace
{
constexpr int kGapBelowChip = 4;
constexpr int kCreateFlowIconPx = 16;
/** Matches CallConfigPopup so Flow call and Call menus share the same card size. */
constexpr int kPopupMinWidth = 300;

/** Multi-color SVG baked to a pixmap so Qlementine won't recolor it to text color. */
QIcon createFlowIcon()
{
  const QString path = iconPathFromTheme(QStringLiteral("button_addflow.svg"));
  QSvgRenderer renderer(path);
  if (!renderer.isValid())
    return {};

  QPixmap pixmap(kCreateFlowIconPx, kCreateFlowIconPx);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);
  renderer.render(&painter, QRectF(0, 0, kCreateFlowIconPx, kCreateFlowIconPx));
  painter.end();
  return QIcon(pixmap);
}
}  // namespace

FlowCallMenu::FlowCallMenu(QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
{
  setObjectName(QStringLiteral("FlowCallMenu"));
  setMinimumWidth(kPopupMinWidth);
  setAutoFillBackground(true);
  oclero::qlementine::QlementineStyle::setAutoIconColor(this, oclero::qlementine::AutoIconColor::None);

  // Same card chrome as CallConfigPopup: popup window + padded StyledPanel + labeled fields.
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(10, 10, 10, 10);
  root->setSpacing(8);

  auto* frame = new QFrame(this);
  frame->setFrameShape(QFrame::StyledPanel);
  auto* frameLayout = new QVBoxLayout(frame);
  frameLayout->setContentsMargins(8, 8, 8, 8);
  frameLayout->setSpacing(8);

  frameLayout->addWidget(new QLabel(tr("Task"), frame));

  mTaskCombo = new QComboBox(frame);
  mTaskCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  frameLayout->addWidget(mTaskCombo);

  frameLayout->addWidget(new QLabel(tr("Flow"), frame));

  mFlowCombo = new QComboBox(frame);
  mFlowCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  oclero::qlementine::QlementineStyle::setAutoIconColor(mFlowCombo, oclero::qlementine::AutoIconColor::None);
  frameLayout->addWidget(mFlowCombo);

  root->addWidget(frame);

  connect(mTaskCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
    if (!mNode || !mView || index < 0)
      return;

    const QString taskName = mTaskCombo->itemText(index);
    const QString taskId = mTaskCombo->itemData(index).toString();

    mTaskCombo->blockSignals(true);
    mFlowCombo->blockSignals(true);

    setTaskData(taskName);
    populateFlowCombo(taskId, mStorage);
    selectFirstFlowAndApply();

    mTaskCombo->blockSignals(false);
    mFlowCombo->blockSignals(false);

    mFlowCombo->setEnabled(!taskId.isEmpty());
    updatePosition(mView);
  });

  connect(mFlowCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
    if (index < 0)
      return;

    if (isCreateFlowItem(index))
    {
      handleCreateFlowRequested();
      return;
    }

    if (!mNode || !mView)
      return;

    const QString flowName = mFlowCombo->itemText(index);
    mFlowCombo->blockSignals(true);
    setFlowData(mNode, flowName);
    updateBlockName(mNode);
    mFlowCombo->blockSignals(false);

    updatePosition(mView);
  });
}

QVector<QPair<QString, QString>> FlowCallMenu::buildTaskOptions(NodeItem* node, SaveInfo* storage)
{
  QVector<QPair<QString, QString>> options;
  if (!node || !storage)
    return options;

  const auto callers = storage->getPossibleCallers(node->id(), Types::PropertyTypes::FLOW_CALL);
  for (const auto& caller : callers)
  {
    const QVariant name = caller->getProperty(ConfigKeys::NAME);
    if (!name.isValid() || name.isNull())
      continue;

    options.append(qMakePair(name.toString(), caller->getid()));
  }

  return options;
}

QStringList FlowCallMenu::buildFlowOptions(const QString& taskId, SaveInfo* storage)
{
  QStringList options;
  if (taskId.isEmpty() || !storage)
    return options;

  const auto flows = storage->getFlowsFromNode(taskId);
  for (const auto& flow : flows)
    options.append(flow->getname());

  return options;
}

std::shared_ptr<FlowSaveInfo> FlowCallMenu::promptNewFlow(QWidget* parent)
{
  if (!parent)
    return nullptr;

  EventDialog dialog(QObject::tr("New flow"), parent);
  dialog.setup(std::make_shared<FlowSaveInfo>());
  if (dialog.exec() != QDialog::Accepted)
    return nullptr;

  auto info = dialog.getInfo();
  if (!info)
    return nullptr;

  const QString name = info->getname().trimmed();
  if (name.isEmpty() || Constants::isReservedMainFlowName(name))
    return nullptr;

  info->setName(name);
  return info;
}

bool FlowCallMenu::resolveFlowCallTarget(NodeItem* node, SaveInfo* storage, QString& outTaskId, QString& outFlowName)
{
  outTaskId.clear();
  outFlowName.clear();

  if (!node || !storage)
    return false;

  const QVariant propValue = node->getProperty(QString::fromLatin1(flow_call::kTaskPropertyId));
  if (!propValue.isValid())
    return false;

  const QJsonObject object = propValue.toJsonObject();
  const QString taskName = object.value(ConfigKeys::DATA).toString();
  if (taskName.isEmpty() || taskName == Constants::EMPTY_COMBO)
    return false;

  const QJsonArray options = object.value(ConfigKeys::OPTIONS).toArray();
  if (options.size() <= flow_call::kFlowOptionIndex)
    return false;

  outFlowName = options.at(flow_call::kFlowOptionIndex).toObject().value(ConfigKeys::DATA).toString();
  if (outFlowName.isEmpty() || outFlowName == Constants::EMPTY_COMBO)
    return false;

  for (const auto& option : buildTaskOptions(node, storage))
  {
    if (option.first != taskName)
      continue;

    outTaskId = option.second;
    return !outTaskId.isEmpty();
  }

  return false;
}

void FlowCallMenu::setTaskData(const QString& taskName)
{
  if (!mNode)
    return;

  const QVariant propValue = mNode->getProperty(QString::fromLatin1(flow_call::kTaskPropertyId));
  if (!propValue.isValid())
    return;

  QJsonObject object = propValue.toJsonObject();
  object[ConfigKeys::DATA] = taskName;
  object[ConfigKeys::OPTIONS] = QJsonArray();
  mNode->setProperty(QString::fromLatin1(flow_call::kTaskPropertyId), object);
}

void FlowCallMenu::setFlowData(NodeItem* node, const QString& flowName) const
{
  if (!node)
    return;

  const QVariant propValue = node->getProperty(QString::fromLatin1(flow_call::kTaskPropertyId));
  if (!propValue.isValid())
    return;

  QJsonObject object = propValue.toJsonObject();
  QJsonArray array = object[ConfigKeys::OPTIONS].toArray();

  QJsonObject item;
  item[ConfigKeys::DATA] = flowName;
  item[ConfigKeys::TYPE] = Types::PropertyTypesToString(Types::PropertyTypes::EVENT_SELECT);
  item[ConfigKeys::IS_VARIABLE] = false;

  if (flow_call::kFlowOptionIndex < array.size())
    array[flow_call::kFlowOptionIndex] = item;
  else
  {
    while (array.size() <= flow_call::kFlowOptionIndex)
      array.append(QJsonObject());
    array[flow_call::kFlowOptionIndex] = item;
  }

  object[ConfigKeys::OPTIONS] = array;
  node->setProperty(QString::fromLatin1(flow_call::kTaskPropertyId), object);
}

void FlowCallMenu::updateBlockName(NodeItem* node) const
{
  if (!node)
    return;

  const QVariant name = node->getProperty(ConfigKeys::NAME);
  if (!name.isValid())
    return;

  // Keep a stable outer label; task/flow are shown on the body / popup.
  node->syncAutoGeneratedName(QStringLiteral("Flow Call"));
}

QString FlowCallMenu::currentTaskId() const
{
  if (!mTaskCombo || mTaskCombo->currentIndex() < 0)
    return QString();

  return mTaskCombo->itemData(mTaskCombo->currentIndex()).toString();
}

bool FlowCallMenu::isCreateFlowItem(int index) const
{
  if (!mFlowCombo || index < 0 || index >= mFlowCombo->count())
    return false;
  return mFlowCombo->itemData(index).toString() == QLatin1String(kCreateFlowItemData);
}

int FlowCallMenu::firstFlowOptionIndex() const
{
  if (!mFlowCombo)
    return -1;

  for (int i = 0; i < mFlowCombo->count(); ++i)
  {
    if (!isCreateFlowItem(i))
      return i;
  }
  return -1;
}

void FlowCallMenu::selectFirstFlowAndApply()
{
  const int flowIndex = firstFlowOptionIndex();
  if (flowIndex < 0)
  {
    mFlowCombo->setCurrentIndex(-1);
    updateBlockName(mNode);
    return;
  }

  mFlowCombo->setCurrentIndex(flowIndex);
  const QString flowName = mFlowCombo->itemText(flowIndex);
  setFlowData(mNode, flowName);
  updateBlockName(mNode);
}

void FlowCallMenu::populateFlowCombo(const QString& taskId, SaveInfo* storage)
{
  mFlowCombo->blockSignals(true);
  mFlowCombo->clear();

  for (const QString& flowName : buildFlowOptions(taskId, storage))
    mFlowCombo->addItem(flowName, flowName);

  if (!taskId.isEmpty())
    mFlowCombo->addItem(createFlowIcon(), tr("Create new flow"), QString::fromLatin1(kCreateFlowItemData));

  mFlowCombo->blockSignals(false);
  mFlowCombo->setEnabled(!taskId.isEmpty());
}

void FlowCallMenu::handleCreateFlowRequested()
{
  // Capture before this Qt::Popup hides. On WebAssembly the popup can close as
  // soon as the combo dropdown closes; hideEvent defers clearTracking so mNode
  // is still valid here. Never setFlowData() with the Create row's label.
  NodeItem* node = mNode;
  CanvasView* view = mView;
  const QString taskId = currentTaskId();
  if (!node || !view || taskId.isEmpty())
    return;

  QWidget* dialogParent = view->window() ? view->window() : static_cast<QWidget*>(view);

  // Leave the combo/popup event stack before QDialog::exec(). Nested popups plus
  // ASYNCIFY's nested event loop are a common wasm crash.
  QTimer::singleShot(0, this, [this, node, taskId, dialogParent]() {
    hideMenu();
    completeCreateFlow(node, taskId, dialogParent);
  });
}

void FlowCallMenu::completeCreateFlow(NodeItem* node, const QString& taskId, QWidget* dialogParent)
{
  if (!node || !node->scene() || taskId.isEmpty() || !dialogParent)
    return;

  const auto info = promptNewFlow(dialogParent);
  if (!info || !node->scene())
    return;

  const QString flowName = info->getname();
  setFlowData(node, flowName);
  updateBlockName(node);
  node->update();

  // Open the new flow on a fresh event-loop turn so tab creation is not still
  // inside QDialog::exec()'s ASYNCIFY nested loop (wasm).
  QTimer::singleShot(0, this, [this, taskId, info]() {
    emit createFlowRequested(taskId, info);
  });
}

void FlowCallMenu::populateCombos(SaveInfo* storage)
{
  if (!mNode)
    return;

  mTaskCombo->blockSignals(true);
  mFlowCombo->blockSignals(true);

  mTaskCombo->clear();

  for (const auto& option : buildTaskOptions(mNode, storage))
    mTaskCombo->addItem(option.first, option.second);

  QString currentTaskName;
  QString currentFlowName;
  QString currentTaskId;

  const QVariant propValue = mNode->getProperty(QString::fromLatin1(flow_call::kTaskPropertyId));
  if (propValue.isValid())
  {
    const QJsonObject object = propValue.toJsonObject();
    currentTaskName = object.value(ConfigKeys::DATA).toString();
    if (currentTaskName == Constants::EMPTY_COMBO)
      currentTaskName.clear();

    const QJsonArray options = object.value(ConfigKeys::OPTIONS).toArray();
    if (options.size() > flow_call::kFlowOptionIndex)
    {
      currentFlowName = options.at(flow_call::kFlowOptionIndex).toObject().value(ConfigKeys::DATA).toString();
      if (currentFlowName == Constants::EMPTY_COMBO)
        currentFlowName.clear();
    }
  }

  if (mTaskCombo->count() == 0)
  {
    mTaskCombo->setEnabled(false);
    mFlowCombo->clear();
    mFlowCombo->setEnabled(false);
    mTaskCombo->blockSignals(false);
    mFlowCombo->blockSignals(false);
    return;
  }

  mTaskCombo->setEnabled(true);

  int taskIndex = currentTaskName.isEmpty() ? -1 : mTaskCombo->findText(currentTaskName);
  if (taskIndex < 0)
  {
    // No saved task (or unknown name): default to the first available task.
    taskIndex = 0;
    currentTaskName = mTaskCombo->itemText(taskIndex);
    currentTaskId = mTaskCombo->itemData(taskIndex).toString();
    setTaskData(currentTaskName);
    currentFlowName.clear();
  }
  else
  {
    currentTaskId = mTaskCombo->itemData(taskIndex).toString();
  }
  mTaskCombo->setCurrentIndex(taskIndex);

  populateFlowCombo(currentTaskId, storage);

  if (currentFlowName.isEmpty())
  {
    selectFirstFlowAndApply();
  }
  else
  {
    const int flowIndex = mFlowCombo->findText(currentFlowName);
    if (flowIndex >= 0 && !isCreateFlowItem(flowIndex))
    {
      mFlowCombo->setCurrentIndex(flowIndex);
    }
    else
    {
      const int createIndex = mFlowCombo->count() - 1;
      if (createIndex >= 0 && isCreateFlowItem(createIndex))
        mFlowCombo->insertItem(createIndex, currentFlowName, currentFlowName);
      else
        mFlowCombo->addItem(currentFlowName, currentFlowName);
      const int recovered = mFlowCombo->findText(currentFlowName);
      if (recovered >= 0 && !isCreateFlowItem(recovered))
        mFlowCombo->setCurrentIndex(recovered);
    }
  }

  mTaskCombo->blockSignals(false);
  mFlowCombo->blockSignals(false);
}

void FlowCallMenu::showForNode(NodeItem* node, CanvasView* view, SaveInfo* storage)
{
  if (!node || !view || !view->viewport())
  {
    hideMenu();
    return;
  }

  mNode = node;
  mView = view;
  mStorage = storage;
  mTrackedNodeId = node->id();

  populateCombos(storage);
  adjustSize();
  updatePosition(view);
  show();
  raise();
}

void FlowCallMenu::updatePosition(CanvasView* view)
{
  if (!mNode || !view || !view->viewport())
    return;

  QRectF anchorRect = mNode->mapRectToScene(mNode->boundingRect());
  if (auto* flowCall = dynamic_cast<FlowCallNode*>(mNode))
  {
    const QRectF chipRect = flowCall->flowChipSceneRect();
    if (chipRect.isValid() && !chipRect.isEmpty())
      anchorRect = chipRect;
  }

  const QPointF anchorScene(anchorRect.center().x(), anchorRect.bottom());
  QPoint viewPos = view->mapFromScene(anchorScene);
  QPoint globalPos = view->viewport()->mapToGlobal(viewPos);
  globalPos.setX(globalPos.x() - width() / 2);
  globalPos.setY(globalPos.y() + kGapBelowChip);
  move(globalPos);
}

void FlowCallMenu::clearTracking()
{
  mNode = nullptr;
  mView = nullptr;
  mStorage = nullptr;
  mTrackedNodeId.clear();
}

void FlowCallMenu::hideMenu()
{
  clearTracking();
  hide();
}

void FlowCallMenu::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  // Defer clearing so QComboBox::activated can still read mNode after this
  // Qt::Popup auto-hides (common when the nested combo list closes on wasm).
  QTimer::singleShot(0, this, [this]() {
    if (!isVisible())
      clearTracking();
  });
}

QString FlowCallMenu::trackedNodeId() const
{
  return mTrackedNodeId;
}
