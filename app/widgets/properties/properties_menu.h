#pragma once

#include <QFrame>
#include <memory>

#include "../menu_base.h"
#include "config.h"
#include "result.h"
#include "save_info.h"
#include "widgets/frame.h"
#include "widgets/widget_factory.h"

class Flow;
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
 * Special control value conventions:
 *
 * SELECT / EVENT_SELECT:
 *   record {
 *     value: <selected value>,
 *     items: list<value>
 *   }
 *
 * COMPONENT_SELECT:
 *   record {
 *     component: string,
 *     event: string,          // optional: event call
 *     flow: string,           // optional: flow call
 *     arguments: list<value>  // optional
 *   }
 *
 * The presence of "flow" selects FLOW_CALL semantics, the presence of "event"
 * selects EVENT_SELECT semantics, and otherwise the component is treated as a
 * TRIGGER_CALL.
 */
class PropertiesMenu : public QFrame, public MenuBase
{
  Q_OBJECT

public:
  PropertiesMenu(QWidget* parent);

  VoidResult start(std::shared_ptr<SaveInfo> storage);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(const QString& nodeId) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

  VoidResult onTransitionSelected(TransitionItem* transition);
  VoidResult onFlowAdded(Flow* flow, NodeItem* node);
  VoidResult onCreateEvent(NodeItem* node);
  VoidResult onFlowRemoved(const QString& flowId, NodeItem* node);

  QLayout* layout() const;

signals:
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
  std::shared_ptr<SaveInfo> mStorage;
  StyledFrame* mFrame = nullptr;

  // Property editors.
  VoidResult loadProperties(NodeItem* node);
  VoidResult loadValueProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadColorProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadSelectProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);
  VoidResult loadComponentSelectProperty(const std::shared_ptr<IParameter>& property, NodeItem* node);

  VoidResult loadCallArguments(const std::shared_ptr<FlowSaveInfo>& call, const QString& propertyId, NodeItem* node, maki::WidgetGroup* group);

  void addCompleter(maki::InputWidget* editor, const QString& nodeId, const koda::types::TypeReference& type, QStringList additionalValues = {});

  // Controls for dynamically defined fields/events.
  VoidResult loadControls(NodeItem* node);
  Result<QTableWidget*> loadEventTable(NodeItem* node);
  VoidResult loadControlAddEvent(QTableWidget* table, NodeItem* node);

  void openEventDialog(QTableWidget* table, NodeItem* node, int row);
  void addEventToTable(QTableWidget* table, int row, const std::shared_ptr<FlowSaveInfo>& event);
  void removeEventFromTable(QTableWidget* table, NodeItem* node);

  void updateBlockName(NodeItem* node, const QString& componentName, const QString& eventName) const;

  std::shared_ptr<IFlow> getFlowConfigOfRow(const NodeItem* node, QTableWidget* table, int row) const;
};