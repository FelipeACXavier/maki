#include "recent_files_menu.h"

#include <QDir>

#include "app_configs.h"

RecentFilesMenu::RecentFilesMenu(QWidget* parent)
    : QMenu(parent)
{
  setMaximumWidth(Constants::MAXIMUM_MENU_WIDTH);
}

void RecentFilesMenu::setRecentFiles(const QStringList& files)
{
  mRecentFiles = files;
  QMetaObject::invokeMethod(this, [this] { rebuild(); }, Qt::QueuedConnection);
}

void RecentFilesMenu::rebuild()
{
  clear();

  if (mRecentFiles.isEmpty())
  {
    auto* emptyAction = addAction(tr("No recent files"));
    emptyAction->setEnabled(false);
    return;
  }

  QFontMetrics fm(font());
  for (const auto& file : mRecentFiles)
  {
    QString text = fm.elidedText(QDir::toNativeSeparators(file), Qt::ElideLeft, Constants::MINIMUM_MENU_WIDTH);
    QAction* action = addAction(text);
    connect(action, &QAction::triggered, this, [this, file] {
      emit fileRequested(file);
    });
  }
}
