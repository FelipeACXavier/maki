#include "flow_call_menu.h"

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>

#include "app_configs.h"
#include "elements/node.h"
#include "event_dialog.h"
#include "flow_info.h"
#include "keys.h"
#include "node_info.h"
#include "save_info.h"
#include "style_helpers.h"
#include "svg_click_button.h"
#include "system/canvas_view.h"
#include "types.h"

namespace
{
constexpr int kGapBelowNode = 8;
constexpr int kCreateFlowIconPx = 16;
}  // namespace

FlowCallMenu::FlowCallMenu(QWidget* parent)
    : QWidget(parent)
{
  setAttribute(Qt::WA_ShowWithoutActivating, true);
  setFocusPolicy(Qt::NoFocus);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(4, 4, 4, 4);
  layout->setSpacing(4);

  auto* taskLabel = new QLabel(tr("Task"), this);
  layout->addWidget(taskLabel);

  mTaskCombo = new QComboBox(this);
  mTaskCombo->setMinimumWidth(160);
  layout->addWidget(mTaskCombo);

  auto* flowHeaderRow = new QHBoxLayout();
  flowHeaderRow->setContentsMargins(0, 0, 0, 0);
  flowHeaderRow->setSpacing(4);

  auto* flowLabel = new QLabel(tr("Flow"), this);
  flowHeaderRow->addWidget(flowLabel);

  mCreateFlowButton = new SvgClickButton(iconPathFromTheme(QStringLiteral("button_addflow.svg")), QSize(kCreateFlowIconPx, kCreateFlowIconPx), this);
  mCreateFlowButton->setToolTip(tr("Create flow"));
  mCreateFlowButton->setEnabled(false);
  flowHeaderRow->addWidget(mCreateFlowButton);
  flowHeaderRow->addStretch();
  layout->addLayout(flowHeaderRow);

  mFlowCombo = new QComboBox(this);
  mFlowCombo->setMinimumWidth(160);
  layout->addWidget(mFlowCombo);

  connect(mTaskCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
    if (!mNode || !mView || index < 0)
      return;

    const QString taskName = mTaskCombo->itemText(index);
    const QString taskId = mTaskCombo->itemData(index).toString();

    mTaskCombo->blockSignals(true);
    mFlowCombo->blockSignals(true);

    setTaskData(taskName);
    populateFlowCombo(taskId, mStorage);
    mFlowCombo->setCurrentIndex(0);
    updateBlockName(taskName, QString());

    mTaskCombo->blockSignals(false);
    mFlowCombo->blockSignals(false);

    mFlowCombo->setEnabled(!taskId.isEmpty());
    updateCreateFlowButtonState();
    updatePosition(mView);
  });

  connect(mCreateFlowButton, &SvgClickButton::clicked, this, [this] {
    if (!mNode || !mView)
      return;

    const QString taskId = currentTaskId();
    if (taskId.isEmpty())
      return;

    const auto info = promptNewFlow(this);
    if (!info)
      return;

    const QString flowName = info->getname();
    if (mFlowCombo->findText(flowName) < 0)
      mFlowCombo->addItem(flowName, flowName);

    const int flowIndex = mFlowCombo->findText(flowName);
    if (flowIndex >= 0)
      mFlowCombo->setCurrentIndex(flowIndex);

    setFlowData(flowName);
    updateBlockName(currentTaskName(), flowName);
    updatePosition(mView);
    emit createFlowRequested(taskId, info);
  });

  connect(mFlowCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
    if (!mNode || !mView || index < 0)
      return;

    const QString flowName = mFlowCombo->itemText(index);
    if (flowName == Constants::EMPTY_COMBO)
      return;

    mFlowCombo->blockSignals(true);
    setFlowData(flowName);
    updateBlockName(currentTaskName(), flowName);
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

  return dialog.getInfo();
}

bool FlowCallMenu::resolveFlowCallTarget(NodeItem* node, SaveInfo* storage, QString& outTaskId, QString& outFlowName)
{
  outTaskId.clear();
  outFlowName.clear();

  if (!node || !storage)
    return false;

  const QVariant propValue = node->getProperty(QString::fromLatin1(kTaskPropertyId));
  if (!propValue.isValid())
    return false;

  const QJsonObject object = propValue.toJsonObject();
  const QString taskName = object.value(ConfigKeys::DATA).toString();
  if (taskName.isEmpty() || taskName == Constants::EMPTY_COMBO)
    return false;

  const QJsonArray options = object.value(ConfigKeys::OPTIONS).toArray();
  if (options.size() <= kTaskPropertyEventIndex)
    return false;

  outFlowName = options.at(kTaskPropertyEventIndex).toObject().value(ConfigKeys::DATA).toString();
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

  const QVariant propValue = mNode->getProperty(QString::fromLatin1(kTaskPropertyId));
  if (!propValue.isValid())
    return;

  QJsonObject object = propValue.toJsonObject();
  object[ConfigKeys::DATA] = taskName;
  object[ConfigKeys::OPTIONS] = QJsonArray();
  mNode->setProperty(QString::fromLatin1(kTaskPropertyId), object);
}

void FlowCallMenu::setFlowData(const QString& flowName)
{
  if (!mNode)
    return;

  const QVariant propValue = mNode->getProperty(QString::fromLatin1(kTaskPropertyId));
  if (!propValue.isValid())
    return;

  QJsonObject object = propValue.toJsonObject();
  QJsonArray array = object[ConfigKeys::OPTIONS].toArray();

  QJsonObject item;
  item[ConfigKeys::DATA] = flowName;
  item[ConfigKeys::TYPE] = Types::PropertyTypesToString(Types::PropertyTypes::EVENT_SELECT);
  item[ConfigKeys::IS_VARIABLE] = false;

  if (kTaskPropertyEventIndex < array.size())
    array[kTaskPropertyEventIndex] = item;
  else
  {
    while (array.size() <= kTaskPropertyEventIndex)
      array.append(QJsonObject());
    array[kTaskPropertyEventIndex] = item;
  }

  object[ConfigKeys::OPTIONS] = array;
  mNode->setProperty(QString::fromLatin1(kTaskPropertyId), object);
}

void FlowCallMenu::updateBlockName(const QString& componentName, const QString& flowName) const
{
  if (!mNode)
    return;

  const QVariant name = mNode->getProperty(ConfigKeys::NAME);
  if (!name.isValid())
    return;

  const QVariant wasAutoGenerated = mNode->getProperty(QStringLiteral("name_auto_generated"));
  if (wasAutoGenerated.isValid() && !wasAutoGenerated.toBool())
    return;

  const QString newName = componentName + (flowName.isEmpty() ? QString() : QStringLiteral(" ") + flowName);
  mNode->setProperty(ConfigKeys::NAME, newName);
}

QString FlowCallMenu::currentTaskId() const
{
  if (!mTaskCombo || mTaskCombo->currentIndex() < 0)
    return QString();

  return mTaskCombo->itemData(mTaskCombo->currentIndex()).toString();
}

QString FlowCallMenu::currentTaskName() const
{
  if (!mTaskCombo || mTaskCombo->currentIndex() < 0)
    return QString();

  const QString name = mTaskCombo->itemText(mTaskCombo->currentIndex());
  return name == Constants::EMPTY_COMBO ? QString() : name;
}

void FlowCallMenu::populateFlowCombo(const QString& taskId, SaveInfo* storage)
{
  mFlowCombo->blockSignals(true);
  mFlowCombo->clear();
  mFlowCombo->addItem(Constants::EMPTY_COMBO, QString());

  for (const QString& flowName : buildFlowOptions(taskId, storage))
    mFlowCombo->addItem(flowName, flowName);

  mFlowCombo->blockSignals(false);
  mFlowCombo->setEnabled(!taskId.isEmpty());
}

void FlowCallMenu::updateCreateFlowButtonState()
{
  if (mCreateFlowButton)
    mCreateFlowButton->setEnabled(!currentTaskId().isEmpty());
}

void FlowCallMenu::populateCombos(SaveInfo* storage)
{
  if (!mNode)
    return;

  mTaskCombo->blockSignals(true);
  mFlowCombo->blockSignals(true);

  mTaskCombo->clear();
  mTaskCombo->addItem(Constants::EMPTY_COMBO, QString());

  for (const auto& option : buildTaskOptions(mNode, storage))
    mTaskCombo->addItem(option.first, option.second);

  QString currentTaskName;
  QString currentFlowName;
  QString currentTaskId;

  const QVariant propValue = mNode->getProperty(QString::fromLatin1(kTaskPropertyId));
  if (propValue.isValid())
  {
    const QJsonObject object = propValue.toJsonObject();
    currentTaskName = object.value(ConfigKeys::DATA).toString();

    const QJsonArray options = object.value(ConfigKeys::OPTIONS).toArray();
    if (options.size() > kTaskPropertyEventIndex)
      currentFlowName = options.at(kTaskPropertyEventIndex).toObject().value(ConfigKeys::DATA).toString();
  }

  if (currentTaskName.isEmpty())
  {
    mTaskCombo->setCurrentIndex(0);
  }
  else
  {
    int taskIndex = mTaskCombo->findText(currentTaskName);
    if (taskIndex < 0)
    {
      mTaskCombo->addItem(currentTaskName, QString());
      taskIndex = mTaskCombo->count() - 1;
    }
    mTaskCombo->setCurrentIndex(taskIndex);
    currentTaskId = mTaskCombo->itemData(taskIndex).toString();
  }

  populateFlowCombo(currentTaskId, storage);

  if (currentFlowName.isEmpty())
  {
    mFlowCombo->setCurrentIndex(0);
  }
  else
  {
    const int flowIndex = mFlowCombo->findText(currentFlowName);
    if (flowIndex >= 0)
      mFlowCombo->setCurrentIndex(flowIndex);
    else
    {
      mFlowCombo->addItem(currentFlowName, currentFlowName);
      mFlowCombo->setCurrentIndex(mFlowCombo->count() - 1);
    }
  }

  mTaskCombo->blockSignals(false);
  mFlowCombo->blockSignals(false);
  updateCreateFlowButtonState();
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

  populateCombos(storage);
  updatePosition(view);
  show();
  raise();
}

void FlowCallMenu::updatePosition(CanvasView* view)
{
  if (!mNode || !view)
    return;

  const QRectF sceneRect = mNode->mapRectToScene(mNode->boundingRect());
  const QPointF anchorScene(sceneRect.center().x(), sceneRect.bottom());
  QPoint viewPos = view->mapFromScene(anchorScene);
  viewPos.setX(viewPos.x() - width() / 2);
  viewPos.setY(viewPos.y() + kGapBelowNode);
  move(viewPos);
}

void FlowCallMenu::hideMenu()
{
  mNode = nullptr;
  mView = nullptr;
  mStorage = nullptr;
  hide();
}

QString FlowCallMenu::trackedNodeId() const
{
  return mNode ? mNode->id() : QString();
}
