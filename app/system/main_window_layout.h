#pragma once

#include <QMainWindow>

#include "logging.h"

class QWidget;
class QSplitter;
class QTabWidget;

class SystemMenu;
class FlowMenu;
class PropertiesMenu;
class FieldsMenu;
class BehaviourMenu;
class QTextBrowser;
class QToolBox;
class QComboBox;

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
  QTabWidget* mCanvasPanel;

  QTabWidget* mBottomPanel;
  QTextBrowser* mInfoText;
  QTextBrowser* mLogText;
  QComboBox* mLogLevelComboBox;

  // === Right Panel ===
  QSplitter* mRightPanel;
  QTabWidget* mNavigationTab;
  SystemMenu* mSystemMenu;
  FlowMenu* mFlowMenu;
  SystemMenu* mInterfaceMenu;

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

  // Help menu

private:
  void buildMainWindow();

  void buildLeftPanel();
  void buildCentralPanel();
  void buildLogTab();
  void buildRightPanel();

  void buildMenuBar();
};
