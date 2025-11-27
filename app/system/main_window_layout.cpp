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
#include <QSplitter>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QVBoxLayout>

// Custom widgets
#include "app_configs.h"
#include "style_helpers.h"
#include "system/canvas_view.h"
#include "theme.h"
#include "widgets/behaviour_menu.h"
#include "widgets/properties/fields_menu.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/structure/flow_menu.h"
#include "widgets/structure/system_menu.h"

MainWindowlayout::MainWindowlayout(QWidget* parent)
    : QMainWindow(parent)
{
  buildMainWindow();
  // qApp->installEventFilter(this);
}

void MainWindowlayout::buildMainWindow()
{
  // Central widget
  mCentralWidget = new QWidget(this);
  QHBoxLayout* mainLayout = new QHBoxLayout(mCentralWidget);

  // Main horizontal splitter
  mMainSplitter = new QSplitter(Qt::Horizontal, mCentralWidget);

  buildLeftPanel();
  buildCentralPanel();
  buildRightPanel();

  mainLayout->addWidget(mMainSplitter);
  mCentralWidget->setLayout(mainLayout);
  setCentralWidget(mCentralWidget);

  buildMenuBar();

  applyTheme();
}

void MainWindowlayout::buildLeftPanel()
{
  mLeftPanel = new QTabWidget();

  mStructureTab = new QWidget();
  QVBoxLayout* structureLayout = new QVBoxLayout(mStructureTab);
  mStructureToolBox = new QToolBox(mStructureTab);
  structureLayout->addWidget(mStructureToolBox);
  mStructureTab->setLayout(structureLayout);
  mLeftPanel->addTab(mStructureTab, tr("Structure"));

  mBehaviourTab = new QWidget();
  QVBoxLayout* behaviourLayout = new QVBoxLayout(mBehaviourTab);
  mBehaviourToolBox = new QToolBox(mBehaviourTab);
  behaviourLayout->addWidget(mBehaviourToolBox);
  mBehaviourTab->setLayout(behaviourLayout);
  mLeftPanel->addTab(mBehaviourTab, tr("Behaviour"));

  mLeftPanel->setTabIcon(0, addIconWithColor(":/icons/cubes.svg", Qt::white));
  mLeftPanel->setTabToolTip(0, tr("Structure"));

  mLeftPanel->setTabIcon(1, addIconWithColor(":/icons/code-branch.svg", Qt::white));
  mLeftPanel->setTabToolTip(1, tr("Component behaviour"));

  mLeftPanel->tabBar()->setIconSize(QSize(18, 18));

  mMainSplitter->addWidget(mLeftPanel);
}

void MainWindowlayout::buildCentralPanel()
{
  mCentralSplitter = new QSplitter(Qt::Vertical);

  QWidget* header = new QWidget();
  QHBoxLayout* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 8, 0, 8);  // top/bottom spacing
  headerLayout->setAlignment(Qt::AlignCenter);

  // Create the big round button
  QPushButton* genButton = new QPushButton("R");
  genButton->setToolTip("Run");
  genButton->setToolTipDuration(2000);
  genButton->setFixedSize(30, 30);

  QComboBox* genOption = new QComboBox();
  genOption->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  genOption->addItem(tr("Generate"), "Generate");
  genOption->addItem(tr("Verify"), "Verify");
  genOption->addItem(tr("Simulate"), "Simulate");

  QPushButton* deployButton = new QPushButton("D");
  deployButton->setToolTip("Deploy");
  deployButton->setToolTipDuration(2000);
  deployButton->setFixedSize(30, 30);

  headerLayout->addWidget(genButton);
  headerLayout->addWidget(genOption);
  // headerLayout->addWidget(simulateButton);
  headerLayout->addWidget(deployButton);

  // ---------------------------------------------
  mCanvasPanel = new QTabWidget();
  mCanvasPanel->setTabsClosable(true);
  mCanvasPanel->setMovable(true);

  CanvasView* canvasView = new CanvasView();
  // canvasView->setMinimumSize(500, 0);

  mCanvasPanel->addTab(canvasView, "System");
  mCanvasPanel->setCurrentWidget(canvasView);

  // Remove the close button from the system tab
  mCanvasPanel->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

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

  // mCentralSplitter->addWidget(mCanvasPanel);

  mBottomPanel = new QTabWidget();

  // ===================================================================
  // Info tab
  mInfoText = new QTextBrowser(mBottomPanel);
  mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mInfoText->setFont(Fonts::Property);

  mBottomPanel->addTab(mInfoText, tr("Info"));

  buildLogTab();

  mCentralSplitter->addWidget(mBottomPanel);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowlayout::buildRightPanel()
{
  mRightPanel = new QSplitter(Qt::Vertical);
  mRightPanel->setMinimumWidth(250);
  mRightPanel->setMaximumWidth(400);

  // Help Menu
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

  mNavigationTab->addTab(mSystemMenu, tr("System"));

  mFlowMenu = new FlowMenu(mNavigationTab);

  QTreeWidgetItem* systemFlows = new QTreeWidgetItem(mFlowMenu);
  systemFlows->setText(0, tr("System flows"));
  mFlowMenu->addTopLevelItem(systemFlows);

  QTreeWidgetItem* componentFlows = new QTreeWidgetItem(mFlowMenu);
  componentFlows->setText(0, tr("Component flows"));
  mFlowMenu->addTopLevelItem(componentFlows);

  mNavigationTab->addTab(mFlowMenu, tr("Flow"));

  // Properties Menu
  mPropertiesTab = new QTabWidget();

  mPropertiesMenu = new PropertiesMenu(mPropertiesTab);
  mPropertiesTab->addTab(mPropertiesMenu, tr("Properties"));

  mFieldsMenu = new FieldsMenu(mPropertiesTab);
  mPropertiesTab->addTab(mFieldsMenu, tr("Events"));

  mBehaviourMenu = new BehaviourMenu(mPropertiesTab);
  mPropertiesTab->addTab(mBehaviourMenu, tr("Behaviour"));

  mRightPanel->addWidget(mNavigationTab);
  mRightPanel->addWidget(mPropertiesTab);

  mMainSplitter->addWidget(mRightPanel);
}

void MainWindowlayout::buildMenuBar()
{
  // === Menu Bar ===
  mMenuBar = new QMenuBar();

  QMenu* file = mMenuBar->addMenu(tr("File"));

  mActionNew = new QAction(tr("New"), this);
  file->addAction(mActionNew);

  mActionOpen = new QAction(tr("Open"), this);
  file->addAction(mActionOpen);

  mActionSave = new QAction(tr("Save"), this);
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(tr("Save As"), this);
  file->addAction(mActionSaveAs);

  QMenu* edit = mMenuBar->addMenu(tr("Edit"));
  QMenu* view = mMenuBar->addMenu(tr("View"));

  QMenu* window = mMenuBar->addMenu(tr("Diagram"));

  mActionGenerate = new QAction(tr("Generate"), this);
  window->addAction(mActionGenerate);

  QMenu* settings = mMenuBar->addMenu(tr("Settings"));

  mOpenAllSettings = new QAction(tr("Open all settings"), this);
  settings->addAction(mOpenAllSettings);

  mGeneratorMenu = new QMenu(tr("Generator"));
  settings->addMenu(mGeneratorMenu);

  QMenu* help = mMenuBar->addMenu(tr("Help"));

  setMenuBar(mMenuBar);
}

void MainWindowlayout::buildLogTab()
{
  QWidget* logContainer = new QWidget(mBottomPanel);
  QVBoxLayout* logLayout = new QVBoxLayout(logContainer);
  logLayout->setContentsMargins(2, 2, 2, 2);

  // Toolbar
  QToolBar* logToolBar = new QToolBar(logContainer);
  logToolBar->setMaximumHeight(28);
  logToolBar->setMovable(false);
  logToolBar->setFloatable(false);
  logToolBar->setFont(Fonts::SmallTab);

  QWidget* group = new QWidget();
  QHBoxLayout* layout = new QHBoxLayout(group);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  // =======================================================================================
  // Clear button
  QToolButton* clearButton = new QToolButton();
  QAction* clearAction = new QAction(tr("Clear"), this);
  clearButton->setDefaultAction(clearAction);

  clearButton->setAutoRaise(true);
  clearButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  clearButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  clearButton->setFont(Fonts::SmallTab);

  connect(clearAction, &QAction::triggered, this, [this]() { mLogText->clear(); });

  // =======================================================================================
  // Log level selector
  mLogLevelComboBox = new QComboBox(logToolBar);
  mLogLevelComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  mLogLevelComboBox->addItem(tr("Error"), QVariant::fromValue(logging::LogLevel::Error));
  mLogLevelComboBox->addItem(tr("Warning"), QVariant::fromValue(logging::LogLevel::Warning));
  mLogLevelComboBox->addItem(tr("Info"), QVariant::fromValue(logging::LogLevel::Info));
  mLogLevelComboBox->addItem(tr("Debug"), QVariant::fromValue(logging::LogLevel::Debugging));

  QLabel* levelLabel = new QLabel(tr("Log level"));
  levelLabel->setContentsMargins(0, 0, 0, 0);
  levelLabel->setAlignment(Qt::AlignVCenter);

  layout->addWidget(levelLabel);
  layout->addWidget(mLogLevelComboBox);
  layout->addWidget(clearButton);

  logToolBar->addWidget(group);

  mLogText = new QTextBrowser(mBottomPanel);
  mLogText->setFont(Fonts::MonoSpace);

  // Assemble the complete tab
  logLayout->addWidget(logToolBar);
  logLayout->addWidget(mLogText);

  mBottomPanel->addTab(logContainer, tr("Log"));
}

void MainWindowlayout::applyTheme()
{
  if (mLeftPanel)
  {
    mLeftPanel->setObjectName("LeftPanel");
    mLeftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mLeftPanel->tabBar()->setExpanding(true);

    auto tabWidth = Config::getValueFromTheme("@left_tab_width");
    auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
    auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

    int count = mLeftPanel->tabBar()->count();
    if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
    {
      int width = (count * tabWidth.toInt()) + (2 * count * tabPadding.toInt()) + (2 * count * tabBorderSize.toInt());
      mLeftPanel->setMaximumWidth(width);
      mLeftPanel->setFixedWidth(width);
    }
    else
    {
      LOG_WARNING("Failed to derive left panel size from theme");
    }
  }

  if (mCanvasPanel)
  {
    mCanvasPanel->setObjectName("CanvasPanel");
    mCanvasPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mCanvasPanel->tabBar()->setExpanding(true);
  }

  if (mBottomPanel)
  {
    mBottomPanel->setObjectName("InfoPanel");
    mBottomPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mBottomPanel->tabBar()->setExpanding(true);
  }

  if (mRightPanel)
  {
    int navigationTabWidth = 0;
    int propertiesTabWidth = 0;

    if (mNavigationTab)
    {
      mNavigationTab->setObjectName("RightPanel");
      mNavigationTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mNavigationTab->tabBar()->setExpanding(true);

      auto tabWidth = Config::getValueFromTheme("@right_tab_width");
      auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
      auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

      int count = mNavigationTab->tabBar()->count();
      if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
      {
        navigationTabWidth = (count * tabWidth.toInt()) + (2 * count * tabPadding.toInt()) + (2 * count * tabBorderSize.toInt());
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

      auto tabWidth = Config::getValueFromTheme("@right_tab_width");
      auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
      auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

      int count = mPropertiesTab->tabBar()->count();
      if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
      {
        propertiesTabWidth = (count * tabWidth.toInt()) + (2 * count * tabPadding.toInt()) + (2 * count * tabBorderSize.toInt());
        mPropertiesTab->setMinimumWidth(propertiesTabWidth);
      }
      else
      {
        LOG_WARNING("Failed to derive properties panel size from theme");
      }
    }

    mRightPanel->setMinimumWidth(std::max(navigationTabWidth, propertiesTabWidth));
  }
}