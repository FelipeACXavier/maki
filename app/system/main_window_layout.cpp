#include "main_window_layout.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QUndoGroup>
#include <QVBoxLayout>

// Custom widgets
#include "app_configs.h"
#include "logging.h"
#include "process_tab.h"
#include "style_helpers.h"
#include "system/canvas_view.h"
#include "theme.h"
#include "widgets/badged_tab_bar.h"
#include "widgets/badged_tab_widget.h"
#include "widgets/log_table_widget.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/section.h"
#include "widgets/structure/file_menu.h"
#include "widgets/structure/system_menu.h"
#include "widgets/widget_factory.h"

static constexpr int MINIMUM_MENU_WIDTH = 250;
static constexpr int MAXIMUM_MENU_WIDTH = 400;

MainWindowLayout::MainWindowLayout(QWidget* parent)
    : QMainWindow(parent)
{
  buildMainWindow();
}

void MainWindowLayout::buildMainWindow()
{
  // Central widget
  mCentralWidget = new QWidget(this);
  QHBoxLayout* mainLayout = new QHBoxLayout(mCentralWidget);

  // Main horizontal splitter
  mMainSplitter = new QSplitter(Qt::Horizontal, mCentralWidget);

  // Application undo group
  mUndoGroup = new QUndoGroup(this);

  buildLeftPanel();
  buildCentralPanel();
  buildRightPanel();

  mainLayout->addWidget(mMainSplitter);
  mCentralWidget->setLayout(mainLayout);
  setCentralWidget(mCentralWidget);

  buildMenuBar();

  applyTheme();
}

void MainWindowLayout::buildLeftPanel()
{
  mLeftPanel = new QSplitter(Qt::Vertical, mMainSplitter);
  mPalette = new QTabWidget(mLeftPanel);

  mStructureTab = new QWidget();
  QVBoxLayout* structureLayout = new QVBoxLayout(mStructureTab);
  structureLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  mStructureTab->setLayout(structureLayout);

  mStructureScrollArea = new QScrollArea(this);
  mStructureScrollArea->setWidgetResizable(true);
  mStructureScrollArea->setFrameShape(QFrame::NoFrame);
  mStructureScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mStructureScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  mStructureScrollArea->setWidget(mStructureTab);

  auto sindex = mPalette->addTab(mStructureScrollArea, tr("Structure"));
  mTranslatable.push_back({mPalette->tabBar(), "Structure", sindex});

  mBehaviourTab = new QWidget();
  QVBoxLayout* behaviourLayout = new QVBoxLayout(mBehaviourTab);
  behaviourLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  mBehaviourTab->setLayout(behaviourLayout);

  mBehaviourScrollArea = new QScrollArea(this);
  mBehaviourScrollArea->setWidgetResizable(true);
  mBehaviourScrollArea->setFrameShape(QFrame::NoFrame);
  mBehaviourScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mBehaviourScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  mBehaviourScrollArea->setWidget(mBehaviourTab);

  auto bindex = mPalette->addTab(mBehaviourScrollArea, tr("Behavior"));
  mTranslatable.push_back({mPalette->tabBar(), "Behavior", bindex});

  mIcons.append({mPalette->tabBar(), ":/icons/structure.svg", 0});
  mIcons.append({mPalette->tabBar(), ":/icons/behaviour.svg", 1});

  mPalette->tabBar()->setIconSize(QSize(18, 18));

  mPaletteSearch = new maki::SearchWidget(tr("Filter nodes"), mLeftPanel);
  mPaletteSearch->hide();
  mIcons.append(mPaletteSearch->icon());

  connect(mPaletteSearch, &maki::SearchWidget::dismissed, this, [this] {
    mPaletteSearch->hide();
    if (mPalette->currentIndex() == 0)
      mStructureScrollArea->setFocus();
    else
      mBehaviourScrollArea->setFocus();
  });

  mLeftPanel->addWidget(mPalette);
  mLeftPanel->addWidget(mPaletteSearch);

  mMainSplitter->addWidget(mLeftPanel);
}

void MainWindowLayout::buildCentralPanel()
{
  mCentralSplitter = new QSplitter(Qt::Vertical);

  QWidget* header = new QWidget();
  QHBoxLayout* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 8, 0, 8);  // top/bottom spacing
  headerLayout->setSpacing(5);
  headerLayout->setAlignment(Qt::AlignCenter);

  // ---------------------------------------------
  mGenerationButton = new QPushButton("");
  mIcons.append({mGenerationButton, ":/icons/verify.svg"});

  mGenerationButton->setToolTip("Verify system");
  mGenerationButton->setToolTipDuration(2000);
  mGenerationButton->setFixedSize(30, 30);

  headerLayout->addWidget(mGenerationButton);

  // ----------------------------------------------------------------
  mSimulateButton = new QPushButton("");
  mIcons.append({mSimulateButton, ":/icons/play.svg"});
  mSimulateButton->setToolTip("Simulate system");
  mSimulateButton->setToolTipDuration(2000);
  mSimulateButton->setFixedSize(30, 30);

  headerLayout->addWidget(mSimulateButton);

  // ----------------------------------------------------------------
  mGeneratorOption = new QComboBox();

  auto generatorOptionsWrapper = createHeaderComboBox(mGeneratorOption, ":/icons/generator.svg", "Verification plugin");
  headerLayout->addWidget(generatorOptionsWrapper);

  // ----------------------------------------------------------------
  mDeployButton = new QPushButton("");
  mIcons.append({mDeployButton, ":/icons/deploy.svg"});

  mDeployButton->setToolTip("Deploy program to selected application");
  mDeployButton->setToolTipDuration(2000);
  mDeployButton->setFixedSize(30, 30);

  headerLayout->addWidget(mDeployButton);

  // ---------------------------------------------
  mCanvasPanel = new QTabWidget();
  mCanvasPanel->setTabsClosable(true);
  mCanvasPanel->setMovable(true);

  CanvasView* canvasView = new CanvasView();

  mCanvasPanel->addTab(canvasView, "System view");
  mCanvasPanel->setCurrentWidget(canvasView);

  // Remove the close button from the system tab
  mCanvasPanel->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

  // Top right buttons
  QWidget* corner = new QWidget();
  auto* layout = new QHBoxLayout(corner);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  // First button
  mBrowserTabButton = new QPushButton(corner);
  mBrowserTabButton->setObjectName("MainWindowButton");
  mBrowserTabButton->setToolTip(tr("Show simulation tab"));
  mTranslatable.push_back({mBrowserTabButton, "Show simulation tab", 0, true});
  connect(mBrowserTabButton, &QPushButton::pressed, [this] { updateProperty(mBrowserTabButton, Config::HAS_ACTIVITY, false); });
  mIcons.append({mBrowserTabButton, ":/icons/display.svg"});

  layout->addWidget(mBrowserTabButton);

  // Add the whole widget to the corner
  mCanvasPanel->setCornerWidget(corner, Qt::TopRightCorner);

  // =================================================================
  QWidget* canvasContainer = new QWidget();
  QVBoxLayout* canvasLayout = new QVBoxLayout(canvasContainer);
  canvasLayout->setContentsMargins(0, 0, 0, 0);
  canvasLayout->setSpacing(0);

  // Add header + tabs
  canvasLayout->addWidget(header);
  canvasLayout->addWidget(mCanvasPanel);

  // Now add the container to the splitter
  mCentralSplitter->addWidget(canvasContainer);

  mBottomPanel = new BadgedTabWidget();
  mBottomPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mBottomPanel->setMinimumHeight(320);

  // ===================================================================
  QWidget* infoContainer = new QWidget(mBottomPanel);
  QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
  infoLayout->setContentsMargins(2, 2, 2, 2);
  infoLayout->setSpacing(0);

  // Info tab
  mInfoText = new QTextBrowser(mBottomPanel);
  mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mInfoText->setFont(Fonts::Property);
  mInfoText->setHtml(createDefaultMessage());

  infoLayout->addWidget(mInfoText);

  auto iindex = mBottomPanel->addTab(infoContainer, tr("Info"));
  mTranslatable.push_back({mBottomPanel->tabBar(), "Info", iindex});
  mIcons.append({mBottomPanel->tabBar(), ":/icons/info.svg", iindex});

  buildLogTab();

  mProcessTab = new ProcessTab(mBottomPanel);
  PROCESS_TAB_INDEX = mBottomPanel->addTab(mProcessTab, tr("Generation"));
  mIcons.append({mBottomPanel->tabBar(), ":/icons/terminal.svg", PROCESS_TAB_INDEX});

  mCentralSplitter->addWidget(mBottomPanel);
  mCentralSplitter->setCollapsible(0, false);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowLayout::buildRightPanel()
{
  mRightPanel = new QSplitter(Qt::Vertical);
  mRightPanel->setMinimumWidth(MINIMUM_MENU_WIDTH);
  mRightPanel->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  // ----------------------------------------------------------------------
  // Navigation Menu
  mNavigationTab = new QTabWidget();

  mSystemMenu = new SystemMenu(mNavigationTab);
  mSystemMenu->setColumnCount(2);
  mSystemMenu->setHeaderLabels({tr("Name"), tr("Type")});
  mSystemMenu->header()->setAlternatingRowColors(true);
  mSystemMenu->header()->setSectionResizeMode(0, QHeaderView::Stretch);

  mSystemMenu->setColumnWidth(1, 150);
  mSystemMenu->header()->setStretchLastSection(false);
  mSystemMenu->header()->setSectionResizeMode(1, QHeaderView::Fixed);
  mSystemMenu->header()->setTextElideMode(Qt::ElideRight);
  mSystemMenu->header()->setSectionsMovable(false);

  auto sindex = mNavigationTab->addTab(mSystemMenu, tr("System"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "System", sindex});
  mIcons.append({mNavigationTab->tabBar(), ":/icons/system.svg", 0});

  mFileMenu = new GeneratedFilesPanel(mNavigationTab);
  auto findex = mNavigationTab->addTab(mFileMenu, tr("Files"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "Files", findex});
  mIcons.append({mNavigationTab->tabBar(), ":/icons/rectangle-list.svg", 1});

  // ----------------------------------------------------------------------
  // Properties Menu
  mPropertiesTab = new QTabWidget();
  mPropertiesTab->setMinimumHeight(500);

  mPropertiesMenu = new PropertiesMenu(mPropertiesTab);
  auto pindex = mPropertiesTab->addTab(mPropertiesMenu, tr("Properties"));
  mTranslatable.push_back({mPropertiesTab->tabBar(), "Properties", pindex});

  mIcons.append({mPropertiesTab->tabBar(), ":/icons/properties.svg", 0});
  mIcons.append({mPropertiesTab->tabBar(), ":/icons/fields.svg", 1});
  mIcons.append({mPropertiesTab->tabBar(), ":/icons/behaviour.svg", 2});

  mNavigationTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mPropertiesTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mRightPanel->addWidget(mNavigationTab);
  mRightPanel->addWidget(mPropertiesTab);

  mMainSplitter->addWidget(mRightPanel);
}

void MainWindowLayout::buildMenuBar()
{
  // === Menu Bar ===
  mMenuBar = new QMenuBar();
  mMenuBar->setNativeMenuBar(true);

  // ----------------------------------------------------------
  // File menu
  QMenu* file = mMenuBar->addMenu(tr("File"));
  mTranslatable.push_back({file, "File"});

  mActionNew = new QAction(tr("New"), this);
  mTranslatable.push_back({mActionNew, "New"});
  file->addAction(mActionNew);

  mActionOpen = new QAction(tr("Open"), this);
  mTranslatable.push_back({mActionOpen, "Open"});
  file->addAction(mActionOpen);

  mActionOpenRecent = file->addMenu(tr("Open Recent"));
  mTranslatable.push_back({mActionOpenRecent, "Open Recent"});
  mActionOpenRecent->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  file->addSeparator();

  mActionSave = new QAction(tr("Save"), this);
  mTranslatable.push_back({mActionSave, "Save"});
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(tr("Save As"), this);
  mTranslatable.push_back({mActionSaveAs, "Save As"});
  file->addAction(mActionSaveAs);

  file->addSeparator();

  mActionImportLibrary = new QAction(tr("Install library"), this);
  mTranslatable.push_back({mActionImportLibrary, "Install library"});
  file->addAction(mActionImportLibrary);

  mActionInstallPlugin = new QAction(tr("Install plugin"), this);
  mTranslatable.push_back({mActionInstallPlugin, "Install plugin"});
  file->addAction(mActionInstallPlugin);

  file->addSeparator();
  mActionExit = new QAction(tr("Exit"), this);
  mTranslatable.push_back({mActionExit, "Exit"});
  file->addAction(mActionExit);

  // ----------------------------------------------------------
  // Edit menu
  QMenu* edit = mMenuBar->addMenu(tr("Edit"));
  mTranslatable.push_back({edit, "Edit"});

  mActionUndo = mUndoGroup->createUndoAction(this, tr("Undo"));
  mTranslatable.push_back({mActionUndo, "Undo"});
  edit->addAction(mActionUndo);

  mActionRedo = mUndoGroup->createRedoAction(this, tr("Redo"));
  mTranslatable.push_back({mActionRedo, "Redo"});
  edit->addAction(mActionRedo);

  edit->addSeparator();

  mActionCopy = new QAction(tr("Copy"), this);
  mTranslatable.push_back({mActionCopy, "Copy"});
  mActionCopy->setEnabled(false);
  edit->addAction(mActionCopy);

  mActionPaste = new QAction(tr("Paste"), this);
  mTranslatable.push_back({mActionPaste, "Paste"});
  mActionPaste->setEnabled(false);
  edit->addAction(mActionPaste);

  mActionCut = new QAction(tr("Cut"), this);
  mTranslatable.push_back({mActionCut, "Cut"});
  mActionCut->setEnabled(false);
  edit->addAction(mActionCut);

  mActionDelete = new QAction(tr("Delete"), this);
  mTranslatable.push_back({mActionDelete, "Delete"});
  mActionDelete->setEnabled(false);
  edit->addAction(mActionDelete);

  // ----------------------------------------------------------
  // View menu
  QMenu* view = mMenuBar->addMenu(tr("View"));
  mTranslatable.push_back({view, "View"});

  mActionZoomIn = new QAction(tr("Zoom In"), this);
  mTranslatable.push_back({mActionZoomIn, "Zoom In"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  mActionZoomOut = new QAction(tr("Zoom Out"), this);
  mTranslatable.push_back({mActionZoomOut, "Zoom Out"});
  mActionZoomOut->setEnabled(false);
  view->addAction(mActionZoomOut);

  mActionResetZoom = new QAction(tr("Reset Zoom"), this);
  mTranslatable.push_back({mActionResetZoom, "Reset Zoom"});
  mActionResetZoom->setEnabled(false);
  view->addAction(mActionResetZoom);

  mActionFitToScreen = new QAction(tr("Fit to Screen"), this);
  mTranslatable.push_back({mActionFitToScreen, "Fit to Screen"});
  mActionFitToScreen->setEnabled(false);
  view->addAction(mActionFitToScreen);

  view->addSeparator();

  mActionZoomIn = new QAction(tr("Toggle Grid"), this);
  mTranslatable.push_back({mActionZoomIn, "Toggle Grid"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  mActionZoomIn = new QAction(tr("Toggle Snap to Grid"), this);
  mTranslatable.push_back({mActionZoomIn, "Toggle Snap to Grid"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  QMenu* showMenu = view->addMenu(tr("Show/Hide"));
  mTranslatable.push_back({showMenu, "Show/Hide"});
  view->addMenu(showMenu);

  mOpenInfoPanel = new QAction(tr("Information panel"), this);
  mTranslatable.push_back({mOpenInfoPanel, "Information panel"});
  mIcons.append({mOpenInfoPanel, ":/icons/invisible.svg"});
  showMenu->addAction(mOpenInfoPanel);
  connect(mOpenInfoPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mBottomPanel, mOpenInfoPanel);
  });

  mOpenComponentsPanel = new QAction(tr("Components panel"), this);
  mTranslatable.push_back({mOpenComponentsPanel, "Components panel"});
  mIcons.append({mOpenComponentsPanel, ":/icons/invisible.svg"});
  showMenu->addAction(mOpenComponentsPanel);
  connect(mOpenComponentsPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mLeftPanel, mOpenComponentsPanel);
  });

  mOpenPropertiesPanel = new QAction(tr("Properties panel"), this);
  mTranslatable.push_back({mOpenPropertiesPanel, "Properties panel"});
  mIcons.append({mOpenPropertiesPanel, ":/icons/invisible.svg"});
  showMenu->addAction(mOpenPropertiesPanel);
  connect(mOpenPropertiesPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mRightPanel, mOpenPropertiesPanel);
  });

  mSpecialTabsMenu = view->addMenu(tr("Special tabs"));
  mTranslatable.push_back({mSpecialTabsMenu, "Special tabs"});
  view->addMenu(mSpecialTabsMenu);

  // ----------------------------------------------------------
  // Diagram menu
  QMenu* window = mMenuBar->addMenu(tr("Diagram"));
  mTranslatable.push_back({window, "Diagram"});

  // mActionAddTask = new QAction(tr("Add Node"), this);
  // view->addAction(mOpenPropertiesPanel);

  // ----------------------------------------------------------
  // Diagram menu
  QMenu* tools = mMenuBar->addMenu(tr("Tools"));
  mTranslatable.push_back({tools, "Tools"});

  mGeneratorMenu = new QMenu(tr("Generator"));
  mTranslatable.push_back({mGeneratorMenu, "Generator"});
  tools->addMenu(mGeneratorMenu);

  mActionGenerate = new QAction(tr("Verify"), this);
  mTranslatable.push_back({mActionGenerate, "Verify"});
  window->addAction(mActionGenerate);

  mActionSimulate = new QAction(tr("Simulate"), this);
  mTranslatable.push_back({mActionSimulate, "Simulate"});
  window->addAction(mActionSimulate);

  // ----------------------------------------------------------
  // Settings menu
  // QMenu* settings = mMenuBar->addMenu(tr("Settings"));

  // ----------------------------------------------------------
  // Help menu
  QMenu* help = mMenuBar->addMenu(tr("Help"));
  mTranslatable.push_back({help, "Help"});

  mActionDocumentation = new QAction(tr("Documentation"), this);
  mTranslatable.push_back({mActionDocumentation, "Documentation"});
  mActionDocumentation->setEnabled(false);
  help->addAction(mActionDocumentation);

  mActionQuickStartGuide = new QAction(tr("Quick Start Guide"), this);
  mTranslatable.push_back({mActionQuickStartGuide, "Quick Start Guide"});
  mActionQuickStartGuide->setEnabled(false);
  help->addAction(mActionQuickStartGuide);

  mOpenAllSettings = new QAction(tr("Open All Settings"), this);
  mTranslatable.push_back({mOpenAllSettings, "Open All Settings"});
  help->addAction(mOpenAllSettings);

  mActionShortcuts = new QAction(tr("Shortcuts"), this);
  mTranslatable.push_back({mActionShortcuts, "Shortcuts"});
  mActionShortcuts->setEnabled(false);
  help->addAction(mActionShortcuts);

  mActionReportIssue = new QAction(tr("Report Issue"), this);
  mTranslatable.push_back({mActionReportIssue, "Report Issue"});
  mActionReportIssue->setEnabled(false);
  help->addAction(mActionReportIssue);

  mAboutAction = new QAction(tr("About"), this);
  mTranslatable.push_back({mAboutAction, "About"});
  mAboutAction->setEnabled(false);
  help->addAction(mAboutAction);

  setMenuBar(mMenuBar);
}

void MainWindowLayout::buildLogTab()
{
  // QWidget* logContainer = new QWidget(mBottomPanel);
  // QVBoxLayout* logLayout = new QVBoxLayout(logContainer);
  // logLayout->setContentsMargins(2, 2, 2, 2);
  // logLayout->setSpacing(0);

  // // Toolbar
  // QToolBar* logToolBar = new QToolBar(logContainer);
  // logToolBar->setObjectName("LogToolBar");
  // logToolBar->setMovable(false);
  // logToolBar->setFloatable(false);
  // logToolBar->setFont(Fonts::SmallTab);

  mLogTable = new LogTableWidget(mBottomPanel);

  // =======================================================================================
  // Clear button
  // QPushButton* clearButton = new QPushButton();
  // clearButton->setToolTip("Clear the logs");
  // clearButton->setToolTipDuration(2000);
  // mIcons.append({clearButton, ":/icons/clear.svg"});

  // connect(clearButton, &QPushButton::pressed, this, [this]() {
  //   mLogText->clear();
  //   mErrorLogText->clear();
  //   mWarningLogText->clear();
  // });

  // =======================================================================================
  // Set the layout
  // QWidget* group = new QWidget();
  // QHBoxLayout* layout = new QHBoxLayout(group);
  // layout->setContentsMargins(0, 0, 0, 0);
  // layout->setSpacing(5);
  // layout->setAlignment(Qt::AlignCenter);

  // layout->addWidget(errorButton);
  // layout->addWidget(warningButton);
  // layout->addStretch();
  // layout->addWidget(clearButton);

  // logToolBar->addWidget(group);

  // logViews->addWidget(mLogText);
  // logViews->addWidget(mErrorLogText);
  // logViews->addWidget(mWarningLogText);

  // TODO: Make these indices less magical
  // connect(errorButton, &QPushButton::toggled, this, [logViews, warningButton](bool checked) {
  //   if (checked)
  //   {
  //     // Highest priority
  //     logViews->setCurrentIndex(1);
  //   }
  //   else
  //   {
  //     if (warningButton->isChecked())
  //       logViews->setCurrentIndex(2);
  //     else
  //       logViews->setCurrentIndex(0);
  //   }
  // });

  // connect(warningButton, &QPushButton::toggled, this, [logViews, errorButton](bool checked) {
  //   if (checked)
  //   {
  //     if (!errorButton->isChecked())
  //       logViews->setCurrentIndex(2);
  //   }
  //   else
  //   {
  //     logViews->setCurrentIndex(0);
  //   }
  // });

  // Assemble the complete tab
  // logLayout->addWidget(logToolBar);
  // logLayout->addSpacing(2);
  // logLayout->addWidget(logViews);
  // logLayout->addWidget(mLogTable);

  LOG_TAB_INDEX = mBottomPanel->addTab(mLogTable, tr("Log"));
  mIcons.append({mBottomPanel->tabBar(), ":/icons/logs.svg", LOG_TAB_INDEX});
}

int MainWindowLayout::setTabBarWidth(QTabBar* bar, int minWidth, int minBorder, int minPadding)
{
  QFontMetrics fm(bar->font());
  int iconSize = bar->iconSize().width();
  int maxWidth = minWidth;

  // Check the width of all tabs and get the largest
  for (int i = 0; i < bar->count(); ++i)
  {
    int textWidth = fm.horizontalAdvance(bar->tabText(i)) + iconSize + (2 * minPadding);
    maxWidth = qMax(textWidth, maxWidth);
  }

  // Set the width of the tabs
  applyStyle(bar, QString("QTabBar::tab {"
                          "  width: %1"
                          "}")
                      .arg(maxWidth));

  int singleBarWidth = (maxWidth) + (2 * minBorder) + (2 * minPadding);

  return bar->count() * singleBarWidth;
}

void MainWindowLayout::applyTheme()
{
  auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
  auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

  if (mLeftPanel && mPalette)
  {
    mLeftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPalette->setObjectName("LeftPanel");
    mPalette->tabBar()->setExpanding(false);

    auto tabWidth = Config::getValueFromTheme("@left_tab_width");
    if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
    {
      int width = setTabBarWidth(mPalette->tabBar(), tabWidth.toInt(), tabPadding.toInt(), tabBorderSize.toInt());
      mPalette->setMinimumWidth(width);
    }
    else
    {
      LOG_WARNING("Failed to derive left panel size from theme");
    }
  }

  if (mCanvasPanel)
  {
    auto tabWidth = Config::getValueFromTheme("@canvas_tab_width");
    if (tabWidth.isValid())
      setTabBarWidth(mCanvasPanel->tabBar(), tabWidth.toInt(), tabPadding.toInt(), tabBorderSize.toInt());

    mCanvasPanel->setObjectName("CanvasPanel");
    mCanvasPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mCanvasPanel->tabBar()->setExpanding(false);
  }

  if (mBottomPanel)
  {
    auto tabWidth = Config::getValueFromTheme("@info_tab_width");
    if (tabWidth.isValid())
      setTabBarWidth(mBottomPanel->tabBar(), tabWidth.toInt(), tabPadding.toInt(), tabBorderSize.toInt());

    mBottomPanel->setObjectName("InfoPanel");
    mBottomPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mBottomPanel->tabBar()->setExpanding(false);
  }

  if (mRightPanel)
  {
    int navigationTabWidth = 0;
    int propertiesTabWidth = 0;

    auto tabWidth = Config::getValueFromTheme("@right_tab_width");

    if (mNavigationTab)
    {
      mNavigationTab->setObjectName("RightPanel");
      mNavigationTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mNavigationTab->tabBar()->setExpanding(false);

      if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
      {
        navigationTabWidth = setTabBarWidth(mNavigationTab->tabBar(), tabWidth.toInt(), tabPadding.toInt(), tabBorderSize.toInt());
        mNavigationTab->setMinimumWidth(navigationTabWidth);
      }
      else
      {
        LOG_WARNING("Failed to derive navigation panel size from theme");
      }
    }

    if (mPropertiesTab)
    {
      mPropertiesTab->setObjectName("RightPanel");
      mPropertiesTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mPropertiesTab->tabBar()->setExpanding(true);

      if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
      {
        propertiesTabWidth = setTabBarWidth(mPropertiesTab->tabBar(), tabWidth.toInt(), tabPadding.toInt(), tabBorderSize.toInt());
        mPropertiesTab->setMinimumWidth(propertiesTabWidth);
      }
      else
      {
        LOG_WARNING("Failed to derive properties panel size from theme");
      }
    }

    mRightPanel->setMinimumWidth(std::max(navigationTabWidth, propertiesTabWidth));

    // Set initial height ratio
    mRightPanel->setSizes({MAXIMUM_MENU_WIDTH, 600});
  }
}

void MainWindowLayout::onThemeChanged(const AppearanceSettings& settings)
{
  updateIconTheme(mIcons);
  mMenuBar->setNativeMenuBar(settings.nativeMenuBar);
}

void MainWindowLayout::onLanguageChanged()
{
  for (auto& item : mTranslatable)
  {
    if (item.widget)
    {
      if (auto label = qobject_cast<QLabel*>(item.widget))
      {
        label->setText(tr(item.text));
      }
      else if (auto button = qobject_cast<QPushButton*>(item.widget))
      {
        if (item.tooltip)
          button->setToolTip(tr(item.text));
      }
      else if (auto tabBar = qobject_cast<QTabBar*>(item.widget))
      {
        if (item.index < tabBar->count())
          tabBar->setTabText(item.index, tr(item.text));
      }
      else if (auto action = qobject_cast<QAction*>(item.widget))
      {
        action->setText(tr(item.text));
      }
      else if (auto menu = qobject_cast<QMenu*>(item.widget))
      {
        menu->setTitle(tr(item.text));
      }
      else
      {
        LOG_WARNING("Unsupported widget: %s", item.widget->metaObject()->className());
      }
    }
  }

  // Reapply theme so the new labels fit, at least initially
  applyTheme();
}

QWidget* MainWindowLayout::createHeaderComboBox(QComboBox* comboBox, const QString& iconPath, const QString& tooltip)
{
  QWidget* wrapper = new QWidget();
  QHBoxLayout* wLayout = new QHBoxLayout(wrapper);
  wLayout->setContentsMargins(0, 0, 0, 0);
  wLayout->setSpacing(0);

  QLabel* iconLabel = new QLabel();
  iconLabel->setObjectName("HeaderButton");
  iconLabel->setToolTip(tooltip);
  iconLabel->setToolTipDuration(2000);
  mIcons.append({iconLabel, iconPath});

  comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  comboBox->setToolTip(tooltip);
  comboBox->setToolTipDuration(2000);
  comboBox->setObjectName("HeaderComboBox");

  wLayout->addWidget(iconLabel);
  wLayout->addWidget(comboBox);

  return wrapper;
}

void MainWindowLayout::toggleGenerationButton(bool running)
{
  auto it = std::find_if(mIcons.begin(), mIcons.end(), [&](const WidgetWithIcon& item) { return item.widget == mGenerationButton; });
  if (it != mIcons.end())
  {
    if (running)
    {
      mGenerationButton->setToolTip("Cancel current generation");
      it->path = ":/icons/pause.svg";
    }
    else
    {
      mGenerationButton->setToolTip("Run with the selected options");
      it->path = ":/icons/verify.svg";
    }
  }

  updateIconTheme(mIcons);
}

void MainWindowLayout::toggleDeployButton(bool running)
{
}

void MainWindowLayout::togglePanelVisibility(QWidget* panel, QAction* action)
{
  if (panel->isHidden())
  {
    panel->show();
    action->setIcon(addIconWithColor(":/icons/invisible.svg", Qt::white));
  }
  else
  {
    panel->hide();
    action->setIcon(addIconWithColor(":/icons/visible.svg", Qt::white));
  }
}
