#pragma once

#include <QMainWindow>

class QWidget;
class QSplitter;
class QTabWidget;

class CanvasView;
class TreeMenu;
class FlowMenu;
class PropertiesMenu;
class FieldsMenu;
class BehaviourMenu;
class QTextBrowser;
class QToolBox;

class MainWindowlayout : public QMainWindow
{
  Q_OBJECT

public:
  MainWindowlayout(QWidget* parent = nullptr);

protected:
  // === Main ===
  QWidget* mCentralWidget;
  QSplitter* mMainSplitter;

  // === Left Panel ===
  QTabWidget* mLeftPanel;
  QWidget* mStructureTab;
  QToolBox* mStructureToolBox;

  QWidget* mBehaviourTab;
  QToolBox* mBehaviourToolBox;

  // === Central Panel ===
  QSplitter* mCentralSplitter;
  CanvasView* mCanvasView;

  QTabWidget* mBottomPanel;
  QTextBrowser* mInfoText;
  QTextBrowser* mLogText;

  // === Right Panel ===
  QSplitter* mRightPanel;
  QTabWidget* mNavigationTab;
  TreeMenu* mSystemMenu;
  FlowMenu* mFlowMenu;
  TreeMenu* mInterfaceMenu;

  QTabWidget* mPropertiesTab;
  PropertiesMenu* mPropertiesMenu;
  FieldsMenu* mFieldsMenu;
  BehaviourMenu* mBehaviourMenu;

  // === Menu Bar ===
  QMenuBar* mMenuBar;
  QMenu* mGeneratorMenu;

  // === Actions ===
  // File menu
  QAction* mActionNew;
  QAction* mActionOpen;
  QAction* mActionSave;
  QAction* mActionSaveAs;

  // Edit menu
  // View menu
  // Diagram menu
  QAction* mActionGenerate;

  // Settings menu
  QAction* mActionSetErrorLevel;
  QAction* mActionSetWarningLevel;
  QAction* mActionSetInfoLevel;
  QAction* mActionSetDebugLevel;

  // Help menu

private:
  void buildMainWindow();

  void buildLeftPanel();
  void buildCentralPanel();
  void buildRightPanel();

  void buildMenuBar();

  // bool eventFilter(QObject* obj, QEvent* event);
};
