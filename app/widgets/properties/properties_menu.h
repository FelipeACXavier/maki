#pragma once

#include <QFrame>
#include <memory>

#include "../menu_base.h"
#include "elements/flow.h"
#include "result.h"
#include "widgets/frame.h"
#include "widgets/widget_factory.h"

class NodeItem;
class QDialog;
class QLayout;
class QStandardItemModel;
class QTableView;
class TransitionItem;

class FlowSaveInfo;
class PropertyInfo;

/**
 * @brief Menu used to edit the properties, fields, events and transitions of a node.
 *
 * Property values are stored uniformly as maki::Value objects exposed through
 * IParameter. TypeReference describes the value type; ControlTypes only selects
 * a specialised editor when the generic ValueEditorFactory is not sufficient.
 *
 */
class PropertiesMenu : public QFrame, public MenuBase
{
  Q_OBJECT

public:
  explicit PropertiesMenu(QWidget* parent);

  VoidResult start();

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(const QString& nodeId) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

  VoidResult onTransitionSelected(TransitionItem* transition);
  VoidResult onFlowAdded(PFlow flow, NodeItem* node);
  Result<std::shared_ptr<FlowSaveInfo>> onCreateEvent(NodeItem* node, bool synchronousCreation);
  VoidResult onFlowRemoved(const QString& flowId, NodeItem* node);

  QLayout* layout() const;

signals:
  void propertyUpdated(const QString& parameterId, const QString& nodeId, const maki::Value& value);
  void openParameter(const QString& parameterId);
  void nodeFocused(const QString& nodeId);
  void flowSelected(const QString& flowId, const QString& nodeId);
  void flowRemoved(const QString& flowId, const QString& nodeId);

private:
  enum TableRole
  {
    IdRole = Qt::UserRole,
    ModifiableRole = Qt::UserRole + 1,
  };

  QString mCurrentNode;
  QDialog* mCurrentDialog = nullptr;
  StyledFrame* mFrame = nullptr;

  // Property editors.
  VoidResult loadProperties(NodeItem* node);
  VoidResult loadValueProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadColorProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadSelectProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadLinkProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadComponentSelectProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);

  VoidResult loadCallArguments(const std::shared_ptr<FlowSaveInfo>& call, const QString& propertyId, NodeItem* node, maki::WidgetGroup* group);

  void addCompleter(maki::InputWidget* editor, const QString& nodeId, const koda::types::TypeReference& type);

  // Controls for dynamically defined fields/events.
  VoidResult loadControls(NodeItem* node);
  Result<QTableWidget*> loadEventTable(NodeItem* node);
  VoidResult loadControlAddEvent(QTableWidget* table, NodeItem* node);

  std::shared_ptr<FlowSaveInfo> openEventDialog(QTableWidget* table, NodeItem* node, int row, bool synchronousCreation = false);
  void addEventToTable(QTableWidget* table, int row, const std::shared_ptr<FlowSaveInfo>& event);
  void removeEventFromTable(QTableWidget* table, NodeItem* node);

  void updateBlockName(NodeItem* node, const QString& componentName, const QString& eventName) const;

  std::shared_ptr<IFlow> getFlowConfigOfRow(const NodeItem* node, QTableWidget* table, int row) const;
};
