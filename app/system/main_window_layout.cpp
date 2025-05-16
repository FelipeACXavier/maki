#include "main_window_layout.h"

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolBox>
#include <QVBoxLayout>

// Custom widgets
#include "style_helpers.h"
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
}

void MainWindowlayout::buildLeftPanel()
{
  mLeftPanel = new QTabWidget();
  mLeftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mLeftPanel->setMinimumWidth(220);
  mLeftPanel->setMaximumWidth(300);

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

  mLeftPanel->tabBar()->setExpanding(true);

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
  mRightPanel->setMinimumWidth(250);
  mRightPanel->setMaximumWidth(400);

  // Help Menu
  mNavigationTab = new QTabWidget();
  mNavigationTab->setMinimumHeight(200);
  mNavigationTab->setMaximumHeight(800);

  mSystemMenu = new TreeMenu(mNavigationTab);
  mNavigationTab->addTab(mSystemMenu, tr("System"));

  mFlowMenu = new FlowMenu(mNavigationTab);
  mNavigationTab->addTab(mFlowMenu, tr("Flow"));

  mInterfaceMenu = new TreeMenu(mNavigationTab);
  mNavigationTab->addTab(mInterfaceMenu, tr("Interface"));

  // Properties Menu
  mPropertiesTab = new QTabWidget();
  mPropertiesTab->setMinimumHeight(400);
  mPropertiesTab->setMaximumHeight(800);

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

// bool MainWindowlayout::eventFilter(QObject* obj, QEvent* event)
// {
//   if (event->type() == QEvent::MouseButtonPress)
//   {
//     QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//     QPoint globalPos = mouseEvent->globalPos();

//     QWidget* clickedWidget = QApplication::widgetAt(globalPos);
//     if (clickedWidget)
//     {
//       qDebug() << "Clicked widget:" << clickedWidget->metaObject()->className()
//                << "objectName:" << clickedWidget->objectName();
//     }

//     // Check if click lands inside tab 0 of mLeftPanel
//     QTabBar* tabBar = mLeftPanel->tabBar();
//     QRect tabRect = tabBar->tabRect(0);
//     QPoint tabBarGlobalPos = tabBar->mapToGlobal(QPoint(0, 0));
//     QRect tabGlobalRect(tabBarGlobalPos, tabRect.size());

//     if (tabGlobalRect.contains(globalPos))
//     {
//       qDebug() << "CLICK WAS ON STRUCTURE TAB (tab 0)";
//     }
//     else
//     {
//       qDebug() << "Click was outside Structure tab";
//     }
//   }

//   return QObject::eventFilter(obj, event);
// }
