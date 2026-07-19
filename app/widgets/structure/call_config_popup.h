#pragma once

#include <QPoint>
#include <QString>
#include <QWidget>

class CallNode;
class CapabilityIconGrid;
class ConfigurationTable;
class SaveInfo;
class QCloseEvent;
class QHideEvent;
class QToolButton;
class QVBoxLayout;
class QLineEdit;

/**
 * Popup editor for a Call node: capability icon grid, mode+event menu, and arguments.
 * Opened from the capability slot or the expand chip.
 */
class CallConfigPopup : public QWidget
{
  Q_OBJECT

public:
  static void open(QWidget* parent,
                   CallNode* node,
                   SaveInfo* storage,
                   const ConfigurationTable* configTable,
                   const QPoint& globalAnchor);

protected:
  void hideEvent(QHideEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

private:
  explicit CallConfigPopup(QWidget* parent = nullptr);

  void setup(CallNode* node, SaveInfo* storage, const ConfigurationTable* configTable);
  void rebuildCapabilityGrid();
  void refreshDetails();
  void applyCapability(const QString& name, const QString& id);
  void applyEvent(const QString& mode, const QString& eventName);
  void rebuildEventMenu(const QString& capabilityId);
  void rebuildArguments(const QString& capabilityId, const QString& eventName);
  void commitArgumentEdit(QLineEdit* edit);
  void flushArgumentEdits();

  CallNode* mNode = nullptr;
  SaveInfo* mStorage = nullptr;
  const ConfigurationTable* mConfigTable = nullptr;

  QString mSelectedCapabilityId;
  QString mSelectedCapabilityName;
  QString mSelectedMode;
  QString mSelectedEventName;

  CapabilityIconGrid* mCapabilityGrid = nullptr;
  QToolButton* mEventButton = nullptr;
  QWidget* mArgsHost = nullptr;
  QVBoxLayout* mArgsLayout = nullptr;
};
