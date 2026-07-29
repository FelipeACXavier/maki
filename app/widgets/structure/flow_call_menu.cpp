#include "flow_call_menu.h"

#include <QComboBox>
#include <QDialog>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QVBoxLayout>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "app_configs.h"
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
constexpr int kGapBelowNode = 8;
constexpr int kCreateFlowIconPx = 16;

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
    : QWidget(parent)
{
  setAttribute(Qt::WA_ShowWithoutActivating, true);
  setFocusPolicy(Qt::NoFocus);
  oclero::qlementine::QlementineStyle::setAutoIconColor(this, oclero::qlementine::AutoIconColor::None);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(4, 4, 4, 4);
  layout->setSpacing(4);

  layout->addWidget(new QLabel(tr("Task"), this));

  mTaskCombo = new QComboBox(this);
  mTaskCombo->setMinimumWidth(160);
  layout->addWidget(mTaskCombo);

  layout->addWidget(new QLabel(tr("Flow"), this));

  mFlowCombo = new QComboBox(this);
  mFlowCombo->setMinimumWidth(160);
  oclero::qlementine::QlementineStyle::setAutoIconColor(mFlowCombo, oclero::qlementine::AutoIconColor::None);
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
    selectFirstFlowAndApply();

    mTaskCombo->blockSignals(false);
    mFlowCombo->blockSignals(false);

    mFlowCombo->setEnabled(!taskId.isEmpty());
    updatePosition(mView);
  });

  connect(mFlowCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
    if (!mNode || !mView || index < 0)
      return;

    if (isCreateFlowItem(index))
    {
      handleCreateFlowRequested();
      return;
    }

    mLastFlowComboIndex = index;

    const QString flowName = mFlowCombo->itemText(index);
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

  return mTaskCombo->itemText(mTaskCombo->currentIndex());
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
    mLastFlowComboIndex = -1;
    updateBlockName(currentTaskName(), QString());
    return;
  }

  mFlowCombo->setCurrentIndex(flowIndex);
  mLastFlowComboIndex = flowIndex;
  const QString flowName = mFlowCombo->itemText(flowIndex);
  setFlowData(flowName);
  updateBlockName(currentTaskName(), flowName);
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
  if (!mNode || !mView)
    return;

  const QString taskId = currentTaskId();
  if (taskId.isEmpty())
    return;

  const auto info = promptNewFlow(this);
  if (!info)
  {
    mFlowCombo->blockSignals(true);
    const int restore = (mLastFlowComboIndex >= 0 && mLastFlowComboIndex < mFlowCombo->count()
                         && !isCreateFlowItem(mLastFlowComboIndex))
                            ? mLastFlowComboIndex
                            : firstFlowOptionIndex();
    mFlowCombo->setCurrentIndex(restore);
    mFlowCombo->blockSignals(false);
    return;
  }

  const QString flowName = info->getname();
  populateFlowCombo(taskId, mStorage);

  const int flowIndex = mFlowCombo->findText(flowName);
  mFlowCombo->blockSignals(true);
  if (flowIndex >= 0)
  {
    mFlowCombo->setCurrentIndex(flowIndex);
    mLastFlowComboIndex = flowIndex;
  }
  mFlowCombo->blockSignals(false);

  setFlowData(flowName);
  updateBlockName(currentTaskName(), flowName);
  updatePosition(mView);
  emit createFlowRequested(taskId, info);
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

  const QVariant propValue = mNode->getProperty(QString::fromLatin1(kTaskPropertyId));
  if (propValue.isValid())
  {
    const QJsonObject object = propValue.toJsonObject();
    currentTaskName = object.value(ConfigKeys::DATA).toString();
    if (currentTaskName == Constants::EMPTY_COMBO)
      currentTaskName.clear();

    const QJsonArray options = object.value(ConfigKeys::OPTIONS).toArray();
    if (options.size() > kTaskPropertyEventIndex)
    {
      currentFlowName = options.at(kTaskPropertyEventIndex).toObject().value(ConfigKeys::DATA).toString();
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
    mLastFlowComboIndex = -1;
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
    if (flowIndex >= 0)
    {
      mFlowCombo->setCurrentIndex(flowIndex);
      mLastFlowComboIndex = flowIndex;
    }
    else
    {
      // Insert recovered name before the trailing "Create new flow" entry.
      const int createIndex = mFlowCombo->count() - 1;
      if (createIndex >= 0 && isCreateFlowItem(createIndex))
        mFlowCombo->insertItem(createIndex, currentFlowName, currentFlowName);
      else
        mFlowCombo->addItem(currentFlowName, currentFlowName);
      const int recovered = mFlowCombo->findText(currentFlowName);
      mFlowCombo->setCurrentIndex(recovered);
      mLastFlowComboIndex = recovered;
    }
  }

  mTaskCombo->blockSignals(false);
  mFlowCombo->blockSignals(false);
  if (mLastFlowComboIndex < 0)
    mLastFlowComboIndex = mFlowCombo->currentIndex();
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
