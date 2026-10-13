#pragma once

#include <QPointer>
#include <QString>
#include <QWidget>

#include "logging.h"

class QGraphicsView;
class QGraphicsItem;
class TourBubble;
class TourOverlay;

namespace tour
{
static const QString QUICK_TOUR_CAPABILTY = "Siren";
static const QString QUICK_TOUR_EVENT = "start";
static const QString QUICK_TOUR_NODE = "Capability Node";
static const QString QUICK_TOUR_NAME = "MyTask";
}  // namespace tour

struct TourStep
{
  static const int Valid = INT32_MAX;
  static const int Invalid = -1;

  enum class AdvanceOn
  {
    Button,
    UserAction
  };

  int id = -1;  // stable, never changes after creation

  // Either target != nullptr (widget), or view+item != nullptr (scene item).
  QPointer<QWidget> target;
  QGraphicsView* view = nullptr;
  QGraphicsItem* item = nullptr;

  QString title;
  QString text;

  AdvanceOn advanceOn = AdvanceOn::Button;
  int fallbackId = -1;  // UserAction failure jumps here (-1 = stay)
  bool allowBack = true;

  bool isAlive() const;
};

class TourGuide : public QObject
{
  Q_OBJECT
public:
  TourGuide(QWidget* topLevel, QObject* parent = nullptr);
  ~TourGuide() override;

  // --- building the pipeline (append) ------------------------------------
  int addStep(QWidget* target, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn = TourStep::AdvanceOn::Button, int fallbackId = -1,
              int expectedId = -1);
  int addItemStep(QGraphicsView* view, QGraphicsItem* item, const QString& title, const QString& text,
                  TourStep::AdvanceOn advanceOn = TourStep::AdvanceOn::Button, int fallbackId = -1, int expectedId = -1);

  void removeStep(int stepId);

  int insertStep(int beforeId, QWidget* target, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn, int fallbackId, int expectedId);
  int insertItemStep(int beforeId, QGraphicsView* view, QGraphicsItem* item, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn,
                     int fallbackId, int expectedId);

  template <typename Sender, typename Signal>
  void connectTriggerSuccess(Sender* sender, Signal signal, std::function<int(const int currentId)> condition)
  {
    if (!sender)
    {
      LOG_ERROR("Tour trigger: null sender");
      return;
    }

    if (!condition)
    {
      LOG_ERROR("Tour trigger: null condition");
      return;
    }

    QObject::connect(sender, signal, this, [this, condition] {
      LOG_INFO("Trigger success: {} with {}", m_active, m_currentId);
      if (!m_active)
        return;

      auto stepId = condition(m_currentId);
      if (stepId != TourStep::Invalid)
        reportStepOutcome(true);
    });
  }

  template <typename Sender, typename Signal>
  void connectTriggerFailure(Sender* sender, Signal signal, std::function<int(const int currentId)> condition)
  {
    if (!condition)
    {
      LOG_ERROR("Tour trigger: null condition");
      return;
    }

    QObject::connect(sender, signal, this, [this, condition] {
      LOG_INFO("Trigger failure: {} with {}", m_active);
      if (!m_active)
        return;

      auto stepId = condition(m_currentId);
      if (stepId != TourStep::Invalid)
      {
        LOG_INFO("Trying to go to step: {}", stepId);
        goTo(stepId);
      }
    });
  }

  void reportStepOutcome(bool success);

  void start(int startId = -1);
  void stop();
  void cancel();

  void goTo(int stepId);
  void advance();
  void back();

  void setStepCount(int steps);
  int currentStepId() const;
  bool isActive() const;

  bool eventFilter(QObject* watched, QEvent* event) override;

signals:
  void finished();
  void cancelled();
  void stepChanged(int stepId);

private:
  QWidget* m_window;
  TourOverlay* m_overlay;
  TourBubble* m_bubble;
  QList<TourStep> m_steps;
  int m_nextId = 0;
  int m_currentId = -1;  // current position tracked BY ID
  bool m_active = false;
  int mStepCount = -1;

  int appendStep(QWidget* target, QGraphicsView* view, QGraphicsItem* item, const QString& title, const QString& text, TourStep::AdvanceOn advanceOn,
                 int fallbackId, int expectedId);
  int insertStep(int beforeId, QGraphicsView* view, QGraphicsItem* item, QWidget* target, const QString& title, const QString& text,
                 TourStep::AdvanceOn advanceOn, int fallbackId, int expectedId);

  int indexOf(int stepId) const;
  void showStep(const TourStep& step);
  void positionBubble();
};