#pragma once

#include <QMainWindow>

class QWidget;
class QSplitter;
class QTabWidget;

class SystemMenu;
class FlowMenu;
class FieldsMenu;
class BehaviourMenu;
class QTextBrowser;
class QToolBox;
class QComboBox;
class QPushButton;
class PropertiesMenu;
class ProcessTab;

#include "style_helpers.h"

class MainWindowLayout : public QMainWindow
{
  Q_OBJECT

public:
  MainWindowLayout(QWidget* parent = nullptr);

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
  QPushButton* mGenerationButton;
  QComboBox* mGenerationOption;
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
  FlowMenu* mFlowMenu;

  QTabWidget* mPropertiesTab;
  PropertiesMenu* mPropertiesMenu;
  FieldsMenu* mFieldsMenu;
  BehaviourMenu* mBehaviourMenu;

  // === Menu Bar ===
  QMenuBar* mMenuBar;

  // === Actions ===
  // File menu
  QAction* mActionNew;
  QAction* mActionOpen;
  QAction* mActionSave;
  QAction* mActionSaveAs;

  // Edit menu
  // View menu
  QAction* mOpenComponentsPanel;
  QAction* mOpenPropertiesPanel;
  QAction* mOpenInfoPanel;

  // Diagram menu
  QAction* mActionGenerate;
  QMenu* mGeneratorMenu;

  // Settings menu
  QAction* mOpenAllSettings;

  // Help menu
  QAction* mAboutAction;

protected:
  void onThemeChanged();

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
};
