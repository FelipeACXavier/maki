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
class PropertiesMenu : public QFrame
{
  Q_OBJECT

public:
  explicit PropertiesMenu(QWidget* parent);

  VoidResult start();

  VoidResult onNodeRemoved(const QString& nodeId);
  VoidResult onNodeSelected(const QString& nodeId, bool selected);

  VoidResult onTransitionSelected(TransitionItem* transition);
  VoidResult onFlowAdded(const QString& flowId, const QString& nodeId);
  Result<std::shared_ptr<FlowSaveInfo>> onCreateEvent(const QString& nodeId, bool synchronousCreation);
  VoidResult onFlowRemoved(const QString& flowId, const QString& nodeId);

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
  EventDispatcher<DocumentEvent>::Subscription mDocumentSubscription;

  VoidResult onNodeModified(const QString& nodeId);

  // Property editors.
  VoidResult loadProperties(const QString& nodeId);
  VoidResult loadValueProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId);
  VoidResult loadColorProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId);
  VoidResult loadSelectProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId);
  VoidResult loadLinkProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId);
  VoidResult loadComponentSelectProperty(const std::shared_ptr<IParameter>& property, const QString& nodeId);

  VoidResult loadCallArguments(const std::shared_ptr<FlowSaveInfo>& call, const QString& propertyId, const QString& nodeId, maki::WidgetGroup* group);

  void addCompleter(maki::InputWidget* editor, const QString& nodeId, const koda::types::TypeReference& type);

  // Controls for dynamically defined fields/events.
  VoidResult loadControls(const QString& nodeId);
  Result<QTableWidget*> loadEventTable(const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig);
  VoidResult loadControlAddEvent(QTableWidget* table, const QString& nodeId);

  std::shared_ptr<FlowSaveInfo> openEventDialog(QTableWidget* table, const QString& nodeId, int row, bool synchronousCreation = false);
  void addEventToTable(QTableWidget* table, int row, const std::shared_ptr<FlowSaveInfo>& event);
  void removeEventFromTable(QTableWidget* table, const QString& nodeId);

  void updateBlockName(PNodeSaveInfo nodeInfo, const QString& componentName, const QString& eventName) const;

  std::shared_ptr<IFlow> getFlowConfigOfRow(PNodeSaveInfo nodeInfo, QTableWidget* table, int row) const;
};
