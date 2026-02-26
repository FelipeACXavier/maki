// GeneratedFilesPanel.h
#pragma once

#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSaveFile>
#include <QShortcut>
#include <QSplitter>
#include <QTextStream>
#include <QToolBar>
#include <QTreeView>
#include <QWidget>

class FileTypeProxyModel;

class GeneratedFilesPanel : public QTreeView
{
  Q_OBJECT
public:
  explicit GeneratedFilesPanel(QWidget* parent = nullptr);

  void setGenerationRoot(const QString& dir);

signals:
  void fileSaved(const QString& path);
  void openExternallyRequested(QPlainTextEdit* textEditor);

private:
  void openPathInEditor(const QString& filePath);
  bool maybeSaveCurrent();
  bool saveCurrent();
  void setCurrentFile(const QString& filePath);
  bool isLikelyTextFile(const QString& filePath) const;
  bool loadFileUtf8(const QString& filePath, QString& outText, QString* outError) const;

  QString mRootDir;
  QString mCurrentFile;

  QFileSystemModel* mModel = nullptr;
  FileTypeProxyModel* mProxy = nullptr;
  // QTreeView* mTree = nullptr;

  QPlainTextEdit* mEditor = nullptr;

  QAction* mSaveAction = nullptr;
  QAction* mRevertAction = nullptr;
  QAction* mOpenExternalAction = nullptr;
};