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
class SvgClickButton;

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

  void populateCombos(SaveInfo* storage);
  void populateFlowCombo(const QString& taskId, SaveInfo* storage);
  QString currentTaskId() const;
  QString currentTaskName() const;
  void setTaskData(const QString& taskName);
  void setFlowData(const QString& flowName);
  void updateBlockName(const QString& componentName, const QString& flowName) const;
  void updateCreateFlowButtonState();

  QComboBox* mTaskCombo = nullptr;
  QComboBox* mFlowCombo = nullptr;
  SvgClickButton* mCreateFlowButton = nullptr;
  NodeItem* mNode = nullptr;
  CanvasView* mView = nullptr;
  SaveInfo* mStorage = nullptr;
};
