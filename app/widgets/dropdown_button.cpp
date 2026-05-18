#include "dropdown_button.h"

#include <qcoreapplication.h>
#include <qsize.h>
#include <qsizepolicy.h>

#include <QActionGroup>
#include <QMenu>

#include "logging.h"

DropDownButton::DropDownButton(QWidget* parent)
    : QToolButton(parent)
{
  mMenu = new QMenu(this);
  mMenu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  setMenu(mMenu);
  setEnabled(true);
  setPopupMode(QToolButton::MenuButtonPopup);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, this, &DropDownButton::showContextMenu);

  connect(this, &QToolButton::clicked, this, [this] {
    if (!mCurrentOption.isEmpty())
      emit executeRequested(mCurrentOption);
  });

  updateButtonText();
}

void DropDownButton::addOption(const QString& name)
{
  LOG_INFO("Adding option: %s", qPrintable(name));
  mOptions.push_back(name);
  rebuildMenu();
}

void DropDownButton::setCurrentOption(const QString& name)
{
  mCurrentOption = name;
  updateButtonText();
}

QString DropDownButton::currentOption()
{
  return mCurrentOption;
}

void DropDownButton::reset()
{
  mOptions.clear();
}

void DropDownButton::rebuildMenu()
{
  mMenu->clear();

  for (const auto& option : mOptions)
  {
    auto* pipelineMenu = mMenu->addMenu(option);
    pipelineMenu->setProperty("id", option);
    buildMenu(pipelineMenu, option, false);
  }

  updateButtonText();
}

void DropDownButton::updateButtonText()
{
  QString optionName;
  for (const auto& option : mOptions)
  {
    if (option != mCurrentOption)
      continue;

    optionName = option;
    break;
  }

  if (optionName.isEmpty())
  {
    if (mOptions.isEmpty())
      optionName = tr("Add new...");
    else
      // mCurrentOption is not longer in options, so we must override it
      optionName = mOptions.first();
  }

  mCurrentOption = optionName;
  setText(optionName);
}

void DropDownButton::showContextMenu(const QPoint& event)
{
  QMenu menu(this);
  buildMenu(&menu, mCurrentOption, true);
  menu.exec(mapToGlobal(event));
}

void DropDownButton::buildMenu(QMenu* menu, const QString& option, bool addRun)
{
  auto* runAction = menu->addAction(tr("Run"));
  auto* selectAction = menu->addAction(tr("Select"));
  menu->addSeparator();

  auto* addAction = menu->addAction(tr("Add..."));
  auto* editAction = menu->addAction(tr("Edit..."));

  connect(runAction, &QAction::triggered, this, [this, option] {
    mCurrentOption = option;
    updateButtonText();
    emit executeRequested(option);
  });
  connect(selectAction, &QAction::triggered, this, [this, option] {
    mCurrentOption = option;
    updateButtonText();
  });
  connect(addAction, &QAction::triggered, this, [this, option] {
    emit editOptionRequested("");
  });
  connect(editAction, &QAction::triggered, this, [this, option] {
    emit editOptionRequested(option);
  });
}