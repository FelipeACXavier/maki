#pragma once

#include <memory>

#include <QPair>
#include <QStringList>
#include <QVector>
#include <QWidget>

class CanvasView;
class FlowSaveInfo;
class NodeItem;
class QComboBox;
class QHideEvent;
class SaveInfo;

/** Call-style popup card below a Flow call node for picking Task and Flow. */
class FlowCallMenu : public QWidget
{
  Q_OBJECT

public:
  explicit FlowCallMenu(QWidget* parent = nullptr);

  void showForNode(NodeItem* node, CanvasView* view, SaveInfo* storage);
  void updatePosition(CanvasView* view);
  void hideMenu();

  QString trackedNodeId() const;

  static QVector<QPair<QString, QString>> buildTaskOptions(NodeItem* node, SaveInfo* storage);
  static QStringList buildFlowOptions(const QString& taskId, SaveInfo* storage);
  static std::shared_ptr<FlowSaveInfo> promptNewFlow(QWidget* parent);
  static bool resolveFlowCallTarget(NodeItem* node, SaveInfo* storage, QString& outTaskId, QString& outFlowName);

signals:
  void createFlowRequested(const QString& taskId, std::shared_ptr<FlowSaveInfo> info);

protected:
  void hideEvent(QHideEvent* event) override;

private:
  /** Sentinel userData for the "Create new flow" entry in the Flow combo. */
  static constexpr const char* kCreateFlowItemData = "__create_new_flow__";

  void populateCombos(SaveInfo* storage);
  void populateFlowCombo(const QString& taskId, SaveInfo* storage);
  QString currentTaskId() const;
  QString currentTaskName() const;
  void setTaskData(const QString& taskName);
  void setFlowData(NodeItem* node, const QString& flowName) const;
  void updateBlockName(NodeItem* node) const;
  void selectFlowInCombo(const QString& flowName);
  bool isCreateFlowItem(int index) const;
  int firstFlowOptionIndex() const;
  void selectFirstFlowAndApply();
  void handleCreateFlowRequested();
  void clearTracking();

  QComboBox* mTaskCombo = nullptr;
  QComboBox* mFlowCombo = nullptr;
  NodeItem* mNode = nullptr;
  CanvasView* mView = nullptr;
  SaveInfo* mStorage = nullptr;
  /** Stable id for move/delete checks without dereferencing mNode after teardown. */
  QString mTrackedNodeId;
  /** Last non-create Flow combo index, used to restore selection if create is cancelled. */
  int mLastFlowComboIndex = -1;
};
