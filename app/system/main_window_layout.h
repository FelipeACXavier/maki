#pragma once

#include <QMainWindow>
#include <oclero/qlementine/widgets/LoadingSpinner.hpp>
#include <oclero/qlementine/widgets/NavigationBar.hpp>

#include "style_helpers.h"
#include "widgets/execute_button.h"
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
class MissionParameterWidget;
class TypeEditor;
}  // namespace maki

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
  QWidget* mCentralWidget;   /// Pointer to the central widget.
  QSplitter* mMainSplitter;  /// Pointer to the main splitter.

  // === Left Panel ===
  QTabWidget* mPalette;               /// Pointer to the palette tab widget.
  StyledFrame* mStructureTab;         /// Pointer to the structure tab frame.
  QScrollArea* mStructureScrollArea;  /// Pointer to the structure scroll area.

  StyledFrame* mBehaviourTab;         /// Pointer to the behaviour tab frame.
  QScrollArea* mBehaviourScrollArea;  /// Pointer to the behaviour scroll area.

  StyledFrame* mPipelineTab;         /// Pointer to the pipeline tab frame.
  QScrollArea* mPipelineScrollArea;  /// Pointer to the pipeline scroll area.

  maki::SearchWidget* mPaletteSearch;  /// Pointer to the palette search widget.
  QSplitter* mLeftPanel;               /// Pointer to the left panel splitter.

  // === Central Panel ===
  ExecuteButton* mPipelineRun;                             /// Pointer to the generator option combo box.
  oclero::qlementine::LoadingSpinner* mGenerationSpinner;  /// Pointer to the generation spinner.

  QSplitter* mCentralSplitter;     /// Pointer to the central splitter.
  QTabWidget* mCanvasPanel;        /// Pointer to the canvas panel tab widget.
  QPushButton* mBrowserTabButton;  /// Pointer to the browser tab button.

  // BadgedTabWidget* mBottomPanel;
  QWidget* mBottomContainer;
  QStackedWidget* mBottomPanel;                          /// Pointer to the bottom panel stacked widget.
  oclero::qlementine::NavigationBar* mBottomNavigation;  /// Pointer to the bottom navigation bar.
  LogTableWidget* mLogTable;                             /// Pointer to the log table widget.
  QTextBrowser* mInfoText;                               /// Pointer to the info text browser.
  ProcessTab* mProcessTab;                               /// Pointer to the process tab.
  maki::MissionParameterWidget* mMissionParameters;
  maki::TypeEditor* mTypeEditor;

  // === Right Panel ===
  QSplitter* mRightPanel;          /// Pointer to the right panel splitter.
  QTabWidget* mNavigationTab;      /// Pointer to the navigation tab widget.
  SystemMenu* mSystemMenu;         /// Pointer to the system menu.
  GeneratedFilesPanel* mFileMenu;  /// Pointer to the file menu.

  QTabWidget* mPropertiesTab;       /// Pointer to the properties tab widget.
  PropertiesMenu* mPropertiesMenu;  /// Pointer to the properties menu.

  // === Menu Bar ===
  QMenuBar* mMenuBar;  /// Pointer to the menu bar.

  // === Actions ===
  // File menu
  QAction* mActionNew;       /// Pointer to the new action.
  QAction* mActionOpen;      /// Pointer to the open action.
  QMenu* mActionOpenRecent;  /// Pointer to the open recent menu.
  QAction* mActionSave;      /// Pointer to the save action.
  QAction* mActionSaveAs;    /// Pointer to the save as action.

  QAction* mActionImportLibrary;  /// Pointer to the import library action.
  QAction* mActionInstallPlugin;  /// Pointer to the install plugin action.

  QAction* mActionExit;  /// Pointer to the exit action.

  // Edit menu
  QUndoGroup* mUndoGroup;  /// Pointer to the undo group.
  QAction* mActionUndo;    /// Pointer to the undo action.
  QAction* mActionRedo;    /// Pointer to the redo action.

  QAction* mActionCopy;    /// Pointer to the copy action.
  QAction* mActionPaste;   /// Pointer to the paste action.
  QAction* mActionCut;     /// Pointer to the cut action.
  QAction* mActionDelete;  /// Pointer to the delete action.

  // View menu
  QAction* mActionZoomIn;        /// Pointer to the zoom in action.
  QAction* mActionZoomOut;       /// Pointer to the zoom out action.
  QAction* mActionResetZoom;     /// Pointer to the reset zoom action.
  QAction* mActionFitToScreen;   /// Pointer to the fit to screen action.
  QAction* mActionToggleToasts;  /// Pointer to the minimize notification toasts action.

  QAction* mOpenComponentsPanel;  /// Pointer to the open components panel action.
  QAction* mOpenPropertiesPanel;  /// Pointer to the open properties panel action.
  QAction* mOpenInfoPanel;        /// Pointer to the open info panel action.

  QMenu* mSpecialTabsMenu;  /// Pointer to the special tabs menu.

  // Diagram menu
  QMenu* mGeneratorMenu;  /// Pointer to the generator menu.
  QAction* mActionAutoRoute;

  QAction* mActionGenerate;  /// Pointer to the generate action.
  QAction* mActionSimulate;  /// Pointer to the simulate action.
  QAction* mActionToggleGrid;
  QAction* mActionToggleSnapToGrid;

  // Help menu
  QAction* mActionDocumentation;    /// Pointer to the documentation action.
  QAction* mActionQuickStartGuide;  /// Pointer to the quick start guide action.
  QAction* mOpenAllSettings;        /// Pointer to the open all settings action.
  QAction* mActionShortcuts;        /// Pointer to the shortcuts action.
  QAction* mActionReportIssue;      /// Pointer to the report issue action.
  QAction* mAboutAction;            /// Pointer to the about action.

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
};
