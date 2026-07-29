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
class SaveInfo;

/** Floating dropdown below a selected Flow call node for picking Task and Flow. */
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

private:
  static constexpr int kTaskPropertyEventIndex = 0;
  static constexpr const char* kTaskPropertyId = "task";
  /** Sentinel userData for the "Create new flow" entry in the Flow combo. */
  static constexpr const char* kCreateFlowItemData = "__create_new_flow__";

  void populateCombos(SaveInfo* storage);
  void populateFlowCombo(const QString& taskId, SaveInfo* storage);
  QString currentTaskId() const;
  QString currentTaskName() const;
  void setTaskData(const QString& taskName);
  void setFlowData(const QString& flowName);
  void updateBlockName(const QString& componentName, const QString& flowName) const;
  bool isCreateFlowItem(int index) const;
  int firstFlowOptionIndex() const;
  void selectFirstFlowAndApply();
  void handleCreateFlowRequested();

  QComboBox* mTaskCombo = nullptr;
  QComboBox* mFlowCombo = nullptr;
  NodeItem* mNode = nullptr;
  CanvasView* mView = nullptr;
  SaveInfo* mStorage = nullptr;
  /** Last non-create Flow combo index, used to restore selection if create is cancelled. */
  int mLastFlowComboIndex = -1;
};
