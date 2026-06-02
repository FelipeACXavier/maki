#pragma once

#include <QMainWindow>
#include <oclero/qlementine/widgets/LoadingSpinner.hpp>
#include <oclero/qlementine/widgets/NavigationBar.hpp>

#include "style_helpers.h"
#include "widgets/dropdown_button.h"
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
class CanvasView;

namespace maki
{
class SearchWidget;
}

/**
 * @brief Main window layout class.
 */
class MainWindowLayout : public QMainWindow
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new MainWindowLayout object.
   *
   * @param parent Pointer to the parent widget.
   */
  MainWindowLayout(QWidget* parent = nullptr);

protected:
  // === Main ===
  QWidget* mCentralWidget = nullptr;   /// Pointer to the central widget.
  QSplitter* mMainSplitter = nullptr;  /// Pointer to the main splitter.

  // === Left Panel ===
  QTabWidget* mPalette = nullptr;               /// Pointer to the palette tab widget.
  StyledFrame* mStructureTab = nullptr;         /// Pointer to the structure tab frame.
  QScrollArea* mStructureScrollArea = nullptr;  /// Pointer to the structure scroll area.

  StyledFrame* mBehaviourTab = nullptr;         /// Pointer to the behaviour tab frame.
  QScrollArea* mBehaviourScrollArea = nullptr;  /// Pointer to the behaviour scroll area.

  StyledFrame* mPipelineTab = nullptr;         /// Pointer to the pipeline tab frame.
  QScrollArea* mPipelineScrollArea = nullptr;  /// Pointer to the pipeline scroll area.

  maki::SearchWidget* mPaletteSearch = nullptr;  /// Pointer to the palette search widget.
  QSplitter* mLeftPanel = nullptr;               /// Pointer to the left panel splitter.

  // === Central Panel ===
  QWidget* mHeaderWidget = nullptr;
  QToolButton* mMenuButton = nullptr;
  DropDownButton* mPipelineRun = nullptr;                            /// Pointer to the generator option combo box.
  oclero::qlementine::LoadingSpinner* mGenerationSpinner = nullptr;  /// Pointer to the generation spinner.

  QSplitter* mCentralSplitter = nullptr;  /// Pointer to the central splitter.
  QTabWidget* mCanvasPanel = nullptr;     /// Pointer to the canvas panel tab widget.

  // BadgedTabWidget* mBottomPanel;
  QWidget* mBottomContainer = nullptr;
  QStackedWidget* mBottomPanel = nullptr;                          /// Pointer to the bottom panel stacked widget.
  oclero::qlementine::NavigationBar* mBottomNavigation = nullptr;  /// Pointer to the bottom navigation bar.
  LogTableWidget* mLogTable = nullptr;                             /// Pointer to the log table widget.
  QTextBrowser* mInfoText = nullptr;                               /// Pointer to the info text browser.
  ProcessTab* mProcessTab = nullptr;                               /// Pointer to the process tab.

  // === Right Panel ===
  QSplitter* mRightPanel = nullptr;          /// Pointer to the right panel splitter.
  QTabWidget* mNavigationTab = nullptr;      /// Pointer to the navigation tab widget.
  SystemMenu* mSystemMenu = nullptr;         /// Pointer to the system menu.
  GeneratedFilesPanel* mFileMenu = nullptr;  /// Pointer to the file menu.

  QTabWidget* mPropertiesTab = nullptr;       /// Pointer to the properties tab widget.
  PropertiesMenu* mPropertiesMenu = nullptr;  /// Pointer to the properties menu.

  // === Menu Bar ===
  QMenuBar* mMenuBar = nullptr;  /// Pointer to the menu bar.

  // === Actions ===
  // File menu
  QAction* mActionNew = nullptr;       /// Pointer to the new action.
  QAction* mActionOpen = nullptr;      /// Pointer to the open action.
  QMenu* mActionOpenRecent = nullptr;  /// Pointer to the open recent menu.
  QAction* mActionSave = nullptr;      /// Pointer to the save action.
  QAction* mActionSaveAs = nullptr;    /// Pointer to the save as action.

  QAction* mActionImportLibrary = nullptr;  /// Pointer to the import library action.
  QAction* mActionInstallPlugin = nullptr;  /// Pointer to the install plugin action.

  QAction* mActionExit = nullptr;  /// Pointer to the exit action.

  // Edit menu
  QUndoGroup* mUndoGroup = nullptr;  /// Pointer to the undo group.
  QAction* mActionUndo = nullptr;    /// Pointer to the undo action.
  QAction* mActionRedo = nullptr;    /// Pointer to the redo action.

  QAction* mActionCopy = nullptr;    /// Pointer to the copy action.
  QAction* mActionPaste = nullptr;   /// Pointer to the paste action.
  QAction* mActionCut = nullptr;     /// Pointer to the cut action.
  QAction* mActionDelete = nullptr;  /// Pointer to the delete action.

  // View menu
  QAction* mActionZoomIn = nullptr;       /// Pointer to the zoom in action.
  QAction* mActionZoomOut = nullptr;      /// Pointer to the zoom out action.
  QAction* mActionResetZoom = nullptr;    /// Pointer to the reset zoom action.
  QAction* mActionFitToScreen = nullptr;  /// Pointer to the fit to screen action.

  QAction* mOpenComponentsPanel = nullptr;  /// Pointer to the open components panel action.
  QAction* mOpenPropertiesPanel = nullptr;  /// Pointer to the open properties panel action.
  QAction* mOpenInfoPanel = nullptr;        /// Pointer to the open info panel action.

  QMenu* mSpecialTabsMenu = nullptr;  /// Pointer to the special tabs menu.

  // Diagram menu
  QMenu* mGeneratorMenu = nullptr;  /// Pointer to the generator menu.
  QAction* mActionAutoRoute = nullptr;

  QAction* mActionGenerate = nullptr;  /// Pointer to the generate action.
  QAction* mActionSimulate = nullptr;  /// Pointer to the simulate action.
  QAction* mActionToggleGrid = nullptr;
  QAction* mActionToggleSnapToGrid = nullptr;

  // Help menu
  QAction* mActionDocumentation = nullptr;    /// Pointer to the documentation action.
  QAction* mActionQuickStartGuide = nullptr;  /// Pointer to the quick start guide action.
  QAction* mOpenAllSettings = nullptr;        /// Pointer to the open all settings action.
  QAction* mActionShortcuts = nullptr;        /// Pointer to the shortcuts action.
  QAction* mActionReportIssue = nullptr;      /// Pointer to the report issue action.
  QAction* mAboutAction = nullptr;            /// Pointer to the about action.

  int LOG_TAB_INDEX = 0;      /// Index of the log tab.
  int PROCESS_TAB_INDEX = 0;  /// Index of the process tab.

protected:
  /**
   * @brief Handles language changed event.
   */
  void onLanguageChanged();

  /**
   * @brief Toggles the generation button state.
   *
   * @param running True if running, false otherwise.
   */
  void toggleGenerationButton(bool running);

private:
  QList<TranslatableWidget> mTranslatable;  /// List of translatable widgets.

  /**
   * @brief Builds the main window layout.
   */
  void buildMainWindow();

  /**
   * @brief Builds the left panel.
   */
  void buildLeftPanel();

  /**
   * @brief Builds the central panel.
   */
  void buildCentralPanel();

  /**
   * @brief Builds the right panel.
   */
  void buildRightPanel();

  /**
   * @brief Builds the menu bar.
   */
  void buildMenuBar();

  /**
   * @brief Applies the current theme.
   */
  void applyTheme();

  /**
   * @brief Creates a header combo box with an icon and tooltip.
   *
   * @param comboBox Pointer to the combo box.
   * @param iconPath Path to the icon image.
   * @param tooltip Tooltip text for the combo box.
   * @return QWidget* Pointer to the created widget.
   */
  QWidget* createHeaderComboBox(QComboBox* comboBox, const QString& iconPath, const QString& tooltip);

  /**
   * @brief Toggles the visibility of a panel based on an action.
   *
   * @param panel Pointer to the panel widget.
   * @param action Pointer to the action that controls the panel's visibility.
   */
  void togglePanelVisibility(QWidget* panel, QAction* action);

  /**
   * @brief Sets the minimum width for a tab bar.
   *
   * @param bar Pointer to the tab bar.
   * @param minWidth Minimum width of the tab bar.
   * @param minBorder Minimum border size.
   * @param minPadding Minimum padding size.
   * @return int The new minimum width of the tab bar.
   */
  int setTabBarWidth(QTabBar* bar, int minWidth, int minBorder, int minPadding);

  QMenu* createFileMenu(QWidget* parent);
  QMenu* createEditMenu(QWidget* parent);
  QMenu* createViewMenu(QWidget* parent);
  QMenu* createDiagramMenu(QWidget* parent);
  QMenu* createToolsMenu(QWidget* parent);
  QMenu* createHelpMenu(QWidget* parent);
};
