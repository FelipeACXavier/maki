#pragma once

#include <qlineedit.h>

#include <QMainWindow>
#include <oclero/qlementine/widgets/LoadingSpinner.hpp>
#include <oclero/qlementine/widgets/NavigationBar.hpp>

#include "style_helpers.h"
#include "widgets/settings_manager.h"

class QWidget;
class QSplitter;
class QTabWidget;

class SystemMenu;
class FlowMenu;
class QTextBrowser;
class QComboBox;
class QPushButton;
class PropertiesMenu;
class ProcessTab;
class QUndoGroup;
class GeneratedFilesPanel;
class SectionWidget;
class QScrollArea;
class BadgedTabWidget;
class LogTableWidget;
class QStackedWidget;
class StyledFrame;

namespace maki
{
class SearchWidget;
}

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
  QTabWidget* mPalette;
  StyledFrame* mStructureTab;
  QScrollArea* mStructureScrollArea;

  StyledFrame* mBehaviourTab;
  QScrollArea* mBehaviourScrollArea;
  maki::SearchWidget* mPaletteSearch;
  QSplitter* mLeftPanel;

  // === Central Panel ===
  QPushButton* mGenerationButton;
  QPushButton* mSimulateButton;
  QComboBox* mGeneratorOption;
  oclero::qlementine::LoadingSpinner* mGenerationSpinner;

  QPushButton* mDeployButton;

  QSplitter* mCentralSplitter;
  QTabWidget* mCanvasPanel;
  QPushButton* mBrowserTabButton;

  // BadgedTabWidget* mBottomPanel;
  QStackedWidget* mBottomPanel;
  oclero::qlementine::NavigationBar* mBottomNavigation;
  LogTableWidget* mLogTable;
  QTextBrowser* mInfoText;
  ProcessTab* mProcessTab;

  // === Right Panel ===
  QSplitter* mRightPanel;
  QTabWidget* mNavigationTab;
  SystemMenu* mSystemMenu;
  GeneratedFilesPanel* mFileMenu;

  QTabWidget* mPropertiesTab;
  PropertiesMenu* mPropertiesMenu;

  // === Menu Bar ===
  QMenuBar* mMenuBar;

  // === Actions ===
  // File menu
  QAction* mActionNew;
  QAction* mActionOpen;
  QMenu* mActionOpenRecent;
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

  QMenu* mSpecialTabsMenu;

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

  int LOG_TAB_INDEX = 0;
  int PROCESS_TAB_INDEX = 0;

protected:
  void onThemeChanged(const AppearanceSettings& settings);
  void onLanguageChanged();

  void toggleGenerationButton(bool running);
  void toggleDeployButton(bool running);

private:
  QList<TranslatableWidget> mTranslatable;

  void buildMainWindow();

  void buildLeftPanel();
  void buildCentralPanel();
  void buildRightPanel();

  void buildMenuBar();

  void applyTheme();
  QWidget* createHeaderComboBox(QComboBox* comboBox, const QString& iconPath, const QString& tooltip);

  void togglePanelVisibility(QWidget* panel, QAction* action);
  int setTabBarWidth(QTabBar* bar, int minWidth, int minBorder, int minPadding);
};
