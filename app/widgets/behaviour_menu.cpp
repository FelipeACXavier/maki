#include "behaviour_menu.h"

#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "behaviour_dialog.h"
#include "behaviour_item.h"
#include "elements/node.h"
#include "logging.h"
#include "system/canvas.h"

BehaviourMenu::BehaviourMenu(QWidget* parent)
    : QFrame(parent)
    , mCurrentDialog(nullptr)
{
  // Set widget layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);

  setGlobalView();
}

VoidResult BehaviourMenu::onNodeSelected(NodeItem* node, bool selected)
{
  if (mCurrentDialog)
    mCurrentDialog->nodeSelected(node, selected);

  return VoidResult();
}

VoidResult BehaviourMenu::onNodeRemoved(NodeItem* node)
{
  return VoidResult();
}

void BehaviourMenu::clear()
{
  if (!layout())
    return;

  // Remove and delete all child widgets
  while (QLayoutItem* item = layout()->takeAt(0))
  {
    if (QWidget* widget = item->widget())
      widget->deleteLater();

    delete item;
  }
}

void BehaviourMenu::setGlobalView()
{
  clear();

  QWidget* parent = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(parent);

  parent->setLayout(controlLayout);
  layout()->addWidget(parent);

  // Create table to hold new fields
  mBehaviourList = new QListWidget(parent);

  mBehaviourList->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(mBehaviourList, &QListWidget::customContextMenuRequested, this, &BehaviourMenu::showContextMenu);
  connect(mBehaviourList, &QListWidget::itemDoubleClicked, this, &BehaviourMenu::editBehaviour);

  addDynamicWidget(mBehaviourList, parent);

  QPushButton* button = new QPushButton(parent);
  connect(button, &QPushButton::pressed, this, &BehaviourMenu::addBehaviour);

  button->setText("Add behaviour");
  controlLayout->addWidget(button);
}

void BehaviourMenu::addDynamicWidget(QWidget* dynamicWidget, QWidget* parent)
{
  // Add dynamic widgets above the parent but below other widgets
  for (int i = 0; i < layout()->count(); ++i)
  {
    QWidget* widget = layout()->itemAt(i)->widget();
    if (widget != parent)
      continue;

    static_cast<QVBoxLayout*>(layout())->insertWidget(i - 1, dynamicWidget);
    break;
  }
}

void BehaviourMenu::addBehaviour()
{
  if (!mBehaviourList)
    return;

  if (!mGetAvailableNodes)
    return;

  // Get nodes
  auto nodes = mGetAvailableNodes();

  // Open the dialog
  mCurrentDialog = new BehaviourDialog("Add behaviour", this);

  mCurrentDialog->setup(nodes);

  connect(mCurrentDialog, &QDialog::accepted, this, &BehaviourMenu::onDialogAccepted);
  connect(mCurrentDialog, &QDialog::rejected, this, &BehaviourMenu::onDialogRejected);

  mCurrentDialog->setAttribute(Qt::WA_DeleteOnClose);
  mCurrentDialog->show();
}

void BehaviourMenu::onDialogAccepted()
{
  if (!mCurrentDialog)
    return;

  // Get the title entered by the user
  QString title = mCurrentDialog->getName();

  // If the title is not empty, add the item to the list
  if (title.isEmpty())
  {
    QMessageBox::warning(this, "Input Error", "Please enter a valid title.");
    return;
  }

  BehaviourItem* behaviour = new BehaviourItem(title);
  mBehaviourList->addItem(behaviour);

  // Add nodes to the behaviour item
  // Create a sub-behaviour in each selected node

  mCurrentDialog->close();
  mCurrentDialog->deleteLater();
}

void BehaviourMenu::onDialogRejected()
{
  mCurrentDialog->close();
  mCurrentDialog->deleteLater();
}

void BehaviourMenu::editBehaviour(QListWidgetItem* item)
{
  if (!item)
    return;

  LOG_INFO("EDITING ITEM");
}

// Slot to show context menu
void BehaviourMenu::showContextMenu(const QPoint& pos)
{
  // Get the item at the position of the mouse click
  QListWidgetItem* item = mBehaviourList->itemAt(pos);

  if (!item)
    return;

  QMenu contextMenu(this);

  QAction* editAction = contextMenu.addAction("Edit");
  QAction* removeAction = contextMenu.addAction("Delete");
  QAction* focusAction = contextMenu.addAction("Toggle focus");

  connect(focusAction, &QAction::triggered, []() {
    LOG_INFO("SETTING FOCUS");
  });

  connect(editAction, &QAction::triggered, [this, item]() {
    editBehaviour(item);
  });

  connect(removeAction, &QAction::triggered, [this, item]() {
    mBehaviourList->takeItem(mBehaviourList->row(item));
  });

  // Show the context menu at the mouse position
  contextMenu.exec(mBehaviourList->viewport()->mapToGlobal(pos));
}
