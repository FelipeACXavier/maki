#pragma once

#include <functional>

#include <QPair>
#include <QVector>
#include <QWidget>

class CanvasView;
class NodeItem;
class QComboBox;
class SaveInfo;
class TransitionItem;

/** Floating dropdown beside a selected behaviour-flow transition for picking its event. */
class TransitionEventMenu : public QWidget
{
  Q_OBJECT

public:
  explicit TransitionEventMenu(QWidget* parent = nullptr);

  void showForTransition(TransitionItem* transition, CanvasView* view, SaveInfo* storage);
  void updatePosition(CanvasView* view);
  void hideMenu();

  QString trackedTransitionId() const;

  /** Returns (display text, data/label) pairs for the transition event selector. */
  static QVector<QPair<QString, QString>> buildOptions(NodeItem* source, SaveInfo* storage);

private:
  void populateCombo(TransitionItem* transition, SaveInfo* storage);
  void disconnectTransitionHooks();

  QComboBox* mCombo = nullptr;
  TransitionItem* mTransition = nullptr;
  CanvasView* mView = nullptr;
  std::function<void(TransitionItem*)> mTransitionDeletedHook;
};
