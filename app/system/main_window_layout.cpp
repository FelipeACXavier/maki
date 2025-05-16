#include "main_window_layout.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolBox>
#include <QVBoxLayout>

// Custom widgets
#include "system/canvas_view.h"
#include "widgets/behaviour_menu.h"
#include "widgets/properties/fields_menu.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/structure/flow_menu.h"
#include "widgets/structure/tree_menu.h"

MainWindowlayout::MainWindowlayout(QWidget* parent)
    : QMainWindow(parent)
{
  buildMainWindow();
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
}

void MainWindowlayout::buildLeftPanel()
{
  mLeftPanel = new QTabWidget();
  mStructureTab = new QWidget();
  QVBoxLayout* structureLayout = new QVBoxLayout(mStructureTab);
  mStructureToolBox = new QToolBox(mStructureTab);
  structureLayout->addWidget(mStructureToolBox);
  mLeftPanel->addTab(mStructureTab, tr("Structure"));

  mBehaviourTab = new QWidget();
  QVBoxLayout* behaviourLayout = new QVBoxLayout(mBehaviourTab);
  mBehaviourToolBox = new QToolBox(mBehaviourTab);
  behaviourLayout->addWidget(mBehaviourToolBox);
  mLeftPanel->addTab(mBehaviourTab, tr("Behaviour"));

  mMainSplitter->addWidget(mLeftPanel);
}

void MainWindowlayout::buildCentralPanel()
{
  mCentralSplitter = new QSplitter(Qt::Vertical);
  mCanvasView = new CanvasView();
  mCanvasView->setMinimumSize(500, 0);
  mCentralSplitter->addWidget(mCanvasView);

  mBottomPanel = new QTabWidget();
  mInfoText = new QTextBrowser(mBottomPanel);
  mBottomPanel->addTab(mInfoText, tr("Info"));

  mLogText = new QTextBrowser(mBottomPanel);
  mLogText->setStyleSheet("QTextBrowser { font-family: monospace; }");
  mBottomPanel->addTab(mLogText, tr("Log"));

  mCentralSplitter->addWidget(mBottomPanel);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowlayout::buildRightPanel()
{
  mRightPanel = new QSplitter(Qt::Vertical);

  // Help Menu
  mNavigationTab = new QTabWidget();

  mSystemMenu = new TreeMenu(mNavigationTab);
  mNavigationTab->addTab(mSystemMenu, tr("System"));

  mFlowMenu = new FlowMenu(mNavigationTab);
  mNavigationTab->addTab(mFlowMenu, tr("Flow"));

  mInterfaceMenu = new TreeMenu(mNavigationTab);
  mNavigationTab->addTab(mInterfaceMenu, tr("Interface"));

  // Properties Menu
  mPropertiesTab = new QTabWidget();

  mPropertiesMenu = new PropertiesMenu(mPropertiesTab);
  mPropertiesTab->addTab(mPropertiesMenu, tr("Properties"));

  mFieldsMenu = new FieldsMenu(mPropertiesTab);
  mPropertiesTab->addTab(mFieldsMenu, tr("Fields"));

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
  QMenu* logLevel = settings->addMenu(tr("Log level"));

  mActionSetErrorLevel = new QAction(tr("Error"), this);
  logLevel->addAction(mActionSetErrorLevel);

  mActionSetWarningLevel = new QAction(tr("Warning"), this);
  logLevel->addAction(mActionSetWarningLevel);

  mActionSetInfoLevel = new QAction(tr("Info"), this);
  logLevel->addAction(mActionSetInfoLevel);

  mActionSetDebugLevel = new QAction(tr("Debug"), this);
  logLevel->addAction(mActionSetDebugLevel);

  mGeneratorMenu = new QMenu(tr("Generator"));
  settings->addMenu(mGeneratorMenu);

  QMenu* help = mMenuBar->addMenu(tr("Help"));

  setMenuBar(mMenuBar);
}
