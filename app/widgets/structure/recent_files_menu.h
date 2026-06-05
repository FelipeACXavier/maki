#pragma once

#include <QMenu>

class RecentFilesMenu : public QMenu
{
  Q_OBJECT

public:
  RecentFilesMenu(QWidget* parent = nullptr);

  void setRecentFiles(const QStringList& files);

signals:
  void fileRequested(const QString& file);

private slots:
  void rebuild();

private:
  QStringList mRecentFiles;
};