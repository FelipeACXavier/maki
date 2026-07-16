#include "transition_event_menu.h"

#include <QComboBox>
#include <QVBoxLayout>

#include "app_configs.h"
#include "elements/node.h"
#include "elements/transition.h"
#include "keys.h"
#include "save_info.h"
#include "system/canvas_view.h"
#include "types.h"

namespace
{
constexpr int kGapBelowTransition = 8;
}  // namespace

TransitionEventMenu::TransitionEventMenu(QWidget* parent)
    : QWidget(parent)
{
  setAttribute(Qt::WA_ShowWithoutActivating, true);
  setFocusPolicy(Qt::NoFocus);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(4, 4, 4, 4);

  mCombo = new QComboBox(this);
  mCombo->setMinimumWidth(140);
  layout->addWidget(mCombo);

  connect(mCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
    if (!mTransition || !mView || index < 0)
      return;

    const QString display = mCombo->itemText(index);
    const QString data = mCombo->itemData(index).toString();

    mCombo->blockSignals(true);
    mTransition->setEvent(display);
    mTransition->setName(data);
    mTransition->updatePath();
    mCombo->blockSignals(false);

    updatePosition(mView);
  });
}

QVector<QPair<QString, QString>> TransitionEventMenu::buildOptions(NodeItem* source, SaveInfo* storage)
{
  QVector<QPair<QString, QString>> options;
  if (!source)
    return options;

  for (const auto& t : source->configTransitions())
    options.append(qMakePair(t.event, t.event));

  options.append(qMakePair(QStringLiteral("on error"), QStringLiteral("on error")));
  options.append(qMakePair(QStringLiteral("on abort"), QStringLiteral("on abort")));

  if (!storage)
    return options;

  const auto callers = storage->getPossibleCallers(source->id(), Types::PropertyTypes::UNKNOWN);
  for (const auto& caller : callers)
  {
    const QVariant name = caller->getProperty(ConfigKeys::NAME);
    if (!name.isValid() || name.isNull())
      continue;

    const auto events = storage->getEventsOfTypeFromNode(caller->getid(), {Types::CallType::OUT});
    for (const auto& event : events)
      options.append(qMakePair(name.toString() + QStringLiteral(".") + event->getname(), QStringLiteral("on")));
  }

  return options;
}

void TransitionEventMenu::disconnectTransitionHooks()
{
  if (mTransition && mTransitionDeletedHook)
  {
    mTransition->transitionDeleted = nullptr;
    mTransitionDeletedHook = nullptr;
  }
}

void TransitionEventMenu::populateCombo(TransitionItem* transition, SaveInfo* storage)
{
  mCombo->blockSignals(true);
  mCombo->clear();
  mCombo->addItem(Constants::EMPTY_COMBO, QString());

  const auto options = buildOptions(transition ? transition->source() : nullptr, storage);
  for (const auto& option : options)
    mCombo->addItem(option.first, option.second);

  if (transition)
  {
    const QString currentEvent = transition->getEvent();
    const int index = mCombo->findText(currentEvent.isEmpty() ? Constants::EMPTY_COMBO : currentEvent);
    if (index >= 0)
      mCombo->setCurrentIndex(index);
    else if (!currentEvent.isEmpty())
    {
      mCombo->addItem(currentEvent, transition->getName());
      mCombo->setCurrentIndex(mCombo->count() - 1);
    }
  }

  mCombo->blockSignals(false);
}

void TransitionEventMenu::showForTransition(TransitionItem* transition, CanvasView* view, SaveInfo* storage)
{
  if (!transition || !view || !view->viewport())
  {
    hideMenu();
    return;
  }

  disconnectTransitionHooks();

  mTransition = transition;
  mView = view;

  populateCombo(transition, storage);

  mTransitionDeletedHook = [this](TransitionItem*) { hideMenu(); };
  mTransition->transitionDeleted = mTransitionDeletedHook;

  updatePosition(view);
  show();
  raise();
}

void TransitionEventMenu::updatePosition(CanvasView* view)
{
  if (!mTransition || !view)
    return;

  const QPainterPath path = mTransition->path();
  if (path.isEmpty())
    return;

  const QPointF anchorScene = mTransition->mapToScene(path.pointAtPercent(0.5));
  QPoint viewPos = view->mapFromScene(anchorScene);
  viewPos.setX(viewPos.x() - width() / 2);
  viewPos.setY(viewPos.y() + kGapBelowTransition);
  move(viewPos);
}

void TransitionEventMenu::hideMenu()
{
  disconnectTransitionHooks();
  mTransition = nullptr;
  mView = nullptr;
  hide();
}

QString TransitionEventMenu::trackedTransitionId() const
{
  return mTransition ? mTransition->id() : QString();
}
