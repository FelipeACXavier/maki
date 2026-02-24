#pragma once

#include <QMainWindow>

class QWidget;
class QSplitter;
class QTabWidget;

class SystemMenu;
class FlowMenu;
// class FieldsMenu;
// class BehaviourMenu;
class QTextBrowser;
class QToolBox;
class QComboBox;
class QPushButton;
class PropertiesMenu;
class ProcessTab;
class QUndoGroup;

#include "style_helpers.h"
#include "widgets/settings_manager.h"

class MainWindowLayout : public QMainWindow
{
  Q_OBJECT

public:
  MainWindowLayout(QWidget* parent = nullptr);

protected:
  // === Main ===
  QWidget* mCentralWidget = nullptr;
  QSplitter* mMainSplitter;

  // === Left Panel ===
  QTabWidget* mLeftPanel;
  QWidget* mStructureTab;
  QToolBox* mStructureToolBox;

  QWidget* mBehaviourTab;
  QToolBox* mBehaviourToolBox;

  // === Central Panel ===
  QPushButton* mGenerationButton;
  QPushButton* mSimulateButton;
  QComboBox* mGeneratorOption;

  QPushButton* mDeployButton;

  QSplitter* mCentralSplitter;
  QTabWidget* mCanvasPanel;
  QPushButton* mProcessTabButton;
  QPushButton* mBrowserTabButton;

  QTabWidget* mBottomPanel;
  QTextBrowser* mInfoText;
  QTextBrowser* mLogText;
  QTextBrowser* mErrorLogText;
  QTextBrowser* mWarningLogText;
  QComboBox* mLogLevelComboBox;

  // === Right Panel ===
  QSplitter* mRightPanel;
  QTabWidget* mNavigationTab;
  SystemMenu* mSystemMenu;

  QTabWidget* mPropertiesTab;
  PropertiesMenu* mPropertiesMenu;

  // === Menu Bar ===
  QMenuBar* mMenuBar;

  // === Actions ===
  // File menu
  QAction* mActionNew;
  QAction* mActionOpen;
  QAction* mActionOpenRecent;
  QAction* mActionSave;
  QAction* mActionSaveAs;

  QAction* mActionImportLibrary;
  QAction* mActionInstallPlugin;

  QAction* mActionExit;

  // Edit menu
  QUndoGroup* mUndoGroup = nullptr;
  QAction* mActionUndo;
  QAction* mActionRedo;

  QAction* mActionCopy;
  QAction* mActionPaste;
  QAction* mActionCut;
  QAction* mActionDelete;

  // View menu
  QAction* mActionZoomIn;
  QAction* mActionZoomOut;
  QAction* mActionResetZoom;
  QAction* mActionFitToScreen;

  QAction* mOpenComponentsPanel;
  QAction* mOpenPropertiesPanel;
  QAction* mOpenInfoPanel;

  QAction* mOpenPluginTab;

  // Diagram menu
  QMenu* mGeneratorMenu;

  QAction* mActionGenerate;
  QAction* mActionSimulate;

  // Help menu
  QAction* mActionDocumentation;
  QAction* mActionQuickStartGuide;
  QAction* mOpenAllSettings;
  QAction* mActionShortcuts;
  QAction* mActionReportIssue;
  QAction* mAboutAction;

protected:
  void onThemeChanged(const AppearanceSettings& settings);

  void toggleGenerationButton(bool running);
  void toggleDeployButton(bool running);

private:
  QList<WidgetWithIcon> mIcons;

  void buildMainWindow();

  void buildLeftPanel();
  void buildCentralPanel();
  void buildLogTab();
  void buildRightPanel();

  void buildMenuBar();

  void applyTheme();
  QWidget* createHeaderComboBox(QComboBox* comboBox, const QString& iconPath, const QString& tooltip);

  void togglePanelVisibility(QWidget* panel, QAction* action);
};
