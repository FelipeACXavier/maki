#include "tour_guide.h"

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPushButton>

#include "tour_bubble.h"
#include "tour_overlay.h"
#include "widgets/dialogs/prompt.h"

bool TourStep::isAlive() const
{
  if (item)
    return view && item->scene();

  if (target)
    return target->isVisible();

  return true;  // centered step (no target) is always alive
}

TourGuide::TourGuide(QWidget* topLevel, QObject* parent)
    : QObject(parent)
    , m_window(topLevel)
{
  m_overlay = new TourOverlay(topLevel, this);
  m_bubble = new TourBubble(topLevel);

  connect(m_bubble->nextButton(), &QPushButton::clicked, this, &TourGuide::advance);
  connect(m_bubble->backButton(), &QPushButton::clicked, this, &TourGuide::back);
  connect(m_bubble->cancelButton(), &QPushButton::clicked, this, &TourGuide::cancel);

  connect(m_overlay, &TourOverlay::spotlightMoved, this, [this](const QRect&) { positionBubble(); });

  m_window->installEventFilter(this);
}

TourGuide::~TourGuide()
{
  stop();
  if (m_overlay)
  {
    delete m_overlay;
    m_overlay = nullptr;
  }

  if (m_bubble)
  {
    delete m_bubble;
    m_bubble = nullptr;
  }
}

// --- building the pipeline (append) ------------------------------------
int TourGuide::addStep(QWidget* target, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn, int fallbackId, int expectedId)
{
  return appendStep(target, nullptr, nullptr, title, text, advanceOn, fallbackId, expectedId);
}

int TourGuide::addItemStep(QGraphicsView* view, QGraphicsItem* item, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn, int fallbackId,
                           int expectedId)
{
  return appendStep(nullptr, view, item, title, text, advanceOn, fallbackId, expectedId);
}

void TourGuide::removeStep(int stepId)
{
  if (stepId < 0)
    return;

  m_steps.removeIf([stepId](const TourStep& step) { return step.id == stepId; });
}

// --- building the pipeline (insert BEFORE an existing step) -------------
// Inserting never invalidates IDs: fallbacks and triggers are resolved by
// ID at navigation time, so everything keeps pointing where it should.
int TourGuide::insertStep(int beforeId, QWidget* target, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn, int fallbackId,
                          int expectedId)
{
  return insertStep(beforeId, nullptr, nullptr, target, title, text, advanceOn, fallbackId, expectedId);
}

int TourGuide::insertItemStep(int beforeId, QGraphicsView* view, QGraphicsItem* item, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn,
                              int fallbackId, int expectedId)
{
  return insertStep(beforeId, view, item, nullptr, title, text, advanceOn, fallbackId, expectedId);
}

// Host application calls this (or the triggers above do it for you).
void TourGuide::reportStepOutcome(bool success)
{
  if (!m_active)
    return;

  const int idx = indexOf(m_currentId);
  if (idx < 0)
    return;

  const TourStep& s = m_steps[idx];
  if (success)
    advance();
  else if (s.fallbackId >= 0)
    goTo(s.fallbackId);
}

// --- navigation ----------------------------------------------------------
void TourGuide::start(int startId)
{
  if (m_steps.isEmpty())
    return;

  m_active = true;
  m_overlay->setGeometry(m_window->rect());
  m_overlay->show();
  m_overlay->raise();

  // Default start = first step in order.
  goTo(startId >= 0 ? startId : m_steps.first().id);
}

void TourGuide::stop()
{
  m_active = false;
  m_overlay->clearSpotlight();
  m_overlay->hide();
  m_bubble->hide();
}

void TourGuide::cancel()
{
  stop();
  emit cancelled();
}

// Jump by stable ID (any step, any time, running or not after start()).
void TourGuide::goTo(int stepId)
{
  const int idx = indexOf(stepId);
  if (!m_active || idx < 0)
    return;

  LOG_DEBUG("Tour guide, trying to go to: {}", stepId);
  m_currentId = stepId;
  showStep(m_steps[idx]);
}

void TourGuide::advance()
{
  if (!m_active)
    return;

  const int idx = indexOf(m_currentId);
  if (idx < 0)
  {  // current step was removed; go to first
    goTo(m_steps.first().id);
    return;
  }

  LOG_DEBUG("Tour advancing from step {} (index {})", m_currentId, idx);
  int next = idx + 1;
  while (next < m_steps.size() && !m_steps[next].isAlive())
  {
    LOG_WARNING("  Skipping dead tour step {}: {}", m_steps[next].id, m_steps[next].title);
    ++next;  // skip dead steps
  }

  if (next >= m_steps.size())
  {
    stop();
    emit finished();
  }
  else
  {
    LOG_DEBUG("Tour advancing to step {}: {}", m_steps[next].id, m_steps[next].title);
    m_currentId = m_steps[next].id;
    showStep(m_steps[next]);
  }
}

void TourGuide::back()
{
  const int idx = indexOf(m_currentId);
  if (idx <= 0)
    return;
  int prev = idx - 1;
  while (prev >= 0 && !m_steps[prev].isAlive())
    --prev;
  if (prev >= 0)
    goTo(m_steps[prev].id);
}

void TourGuide::setStepCount(int steps)
{
  mStepCount = steps;
}

int TourGuide::currentStepId() const
{
  return m_currentId;
}

bool TourGuide::isActive() const
{
  return m_active;
}

bool TourGuide::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == m_window && (event->type() == QEvent::Move || event->type() == QEvent::Resize))
  {
    m_overlay->setGeometry(m_window->rect());
    positionBubble();
  }
  return QObject::eventFilter(watched, event);
}

// --- internal step construction -------------------------------------------
int TourGuide::appendStep(QWidget* target, QGraphicsView* view, QGraphicsItem* item, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn,
                          int fallbackId, int expectedId)
{
  TourStep s;
  s.id = (expectedId == TourStep::Invalid) ? m_nextId++ : expectedId;
  s.target = target;
  s.view = view;
  s.item = item;
  s.title = title;
  s.text = text;
  s.advanceOn = advanceOn;
  s.fallbackId = fallbackId;
  s.allowBack = true;

  if (expectedId == TourStep::Invalid)
  {
    m_steps.append(s);
    return s.id;
  }

  for (auto& existing : m_steps)
    if (existing.id == expectedId)
    {
      LOG_DEBUG("Updating step: {}", expectedId);
      existing = s;
      break;
    }

  return s.id;
}

int TourGuide::insertStep(int beforeId, QGraphicsView* view, QGraphicsItem* item, QWidget* target, const QString& title, const QString& text,
                          TourStep::AdvanceOn advanceOn, int fallbackId, int expectedId)
{
  TourStep s;
  s.id = (expectedId == TourStep::Invalid) ? m_nextId++ : expectedId;
  s.target = target;
  s.view = view;
  s.item = item;
  s.title = title;
  s.text = text;
  s.advanceOn = advanceOn;
  s.fallbackId = fallbackId;
  s.allowBack = true;

  if (expectedId == TourStep::Invalid)
  {
    const int pos = indexOf(beforeId);
    if (pos < 0)
      m_steps.append(s);
    else
      m_steps.insert(pos, s);

    return s.id;
  }
  for (auto& existing : m_steps)
    if (existing.id == expectedId)
    {
      LOG_DEBUG("Updating item step: {}", expectedId);
      existing = s;
      break;
    }

  return s.id;
}

int TourGuide::indexOf(int stepId) const
{
  for (int i = 0; i < m_steps.size(); ++i)
    if (m_steps[i].id == stepId)
      return i;
  return -1;
}

void TourGuide::showStep(const TourStep& step)
{
  LOG_DEBUG("Showing tour step:");
  LOG_DEBUG(" Target: {}", step.target ? step.target->metaObject()->className() : "None");
  LOG_DEBUG(" View: {}", step.view != nullptr);
  LOG_DEBUG(" Item: {}", step.item != nullptr);
  LOG_DEBUG(" Title: {}", step.title);
  LOG_DEBUG(" Text: {}", step.text);
  LOG_DEBUG(" Fallback: {}", step.fallbackId);
  LOG_DEBUG(" Allow back: {}", step.allowBack);

  if (step.item && step.view)
    m_overlay->setSpotlightItem(step.view, step.item);
  else if (step.target)
    m_overlay->setSpotlightWidget(step.target);
  else
    m_overlay->clearSpotlight();

  const int idx = indexOf(step.id);
  const bool last = (idx == m_steps.size() - 1);
  m_bubble->setContent(step.title, step.text, idx + 1, mStepCount, last, step.allowBack && idx > 0, step.advanceOn == TourStep::AdvanceOn::Button);
  m_bubble->show();
  m_bubble->raise();
  positionBubble();
  emit stepChanged(step.id);
}

void TourGuide::positionBubble()
{
  const QRect hole = m_overlay->spotlightRectLocal();
  const QSize size = m_bubble->size();
  constexpr int margin = 16;

  QPoint pos;

  if (hole.isNull())
  {
    // Center the bubble within the main window.
    pos = m_window->mapToGlobal(m_window->rect().center()) - QPoint(size.width() / 2, size.height() / 2);
  }
  else
  {
    // Convert the spotlight rectangle to global coordinates.
    const QRect globalHole(m_overlay->mapToGlobal(hole.topLeft()), hole.size());

    QScreen* screen = QGuiApplication::screenAt(globalHole.center());
    if (!screen)
      screen = QGuiApplication::primaryScreen();

    if (!screen)
      return;

    const QRect avail = screen->availableGeometry();

    // Prefer positioning the bubble to the right of the target,
    // vertically centered.
    pos.setX(globalHole.right() + margin);
    pos.setY(globalHole.center().y() - size.height() / 2);

    // If there isn't enough space on the right, place it on the left.
    if (pos.x() + size.width() > avail.right())
      pos.setX(globalHole.left() - size.width() - margin);

    // Keep the bubble inside the available screen area.
    pos.setX(qBound(avail.left(), pos.x(), qMax(avail.left(), avail.right() - size.width() + 1)));
    pos.setY(qBound(avail.top(), pos.y(), qMax(avail.top(), avail.bottom() - size.height() + 1)));
  }

  m_bubble->move(pos);
}