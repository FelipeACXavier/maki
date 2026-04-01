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

/**
 * @class GeneratedFilesPanel
 * @brief A tree view panel for browsing and editing generated files.
 *
 * This widget provides:
 * - A file system view rooted at a generation directory
 * - An integrated text editor for viewing and editing files
 * - Save, revert, and open externally actions
 *
 * It is typically used to inspect generated artefacts and optionally edit them.
 */
class GeneratedFilesPanel : public QTreeView
{
  Q_OBJECT
public:
  /**
   * @brief Constructs the generated files panel.
   * @param parent The parent widget.
   */
  explicit GeneratedFilesPanel(QWidget* parent = nullptr);

  /**
   * @brief Sets the root directory for generated files.
   *
   * Updates the file system model and refreshes the view accordingly.
   *
   * @param dir The directory path to use as root.
   */
  void setGenerationRoot(const QString& dir);

signals:
  /**
   * @brief Emitted when a file has been successfully saved.
   * @param path The path of the saved file.
   */
  void fileSaved(const QString& path);

  /**
   * @brief Emitted when the user requests to open the current file externally.
   * @param textEditor The editor containing the file content.
   */
  void openExternallyRequested(QPlainTextEdit* textEditor);

private:
  /**
   * @brief Opens a file in the internal editor.
   * @param filePath The path of the file to open.
   */
  void openPathInEditor(const QString& filePath);

  /**
   * @brief Prompts the user to save changes if the current file is modified.
   * @return True if it is safe to proceed (saved or discarded), false otherwise.
   */
  bool maybeSaveCurrent();

  /**
   * @brief Saves the current file to disk.
   * @return True if the file was successfully saved, false otherwise.
   */
  bool saveCurrent();

  /**
   * @brief Sets the currently active file in the editor.
   * @param filePath The path of the file to set as current.
   */
  void setCurrentFile(const QString& filePath);

  /**
   * @brief Heuristically determines whether a file is likely a text file.
   * @param filePath The path of the file to check.
   * @return True if the file is likely text, false otherwise.
   */
  bool isLikelyTextFile(const QString& filePath) const;

  /**
   * @brief Loads a file as UTF-8 text.
   * @param filePath The path of the file to load.
   * @param outText Output parameter containing the file contents.
   * @param outError Optional output parameter containing an error message.
   * @return True if the file was successfully loaded, false otherwise.
   */
  bool loadFileUtf8(const QString& filePath, QString& outText, QString* outError) const;

  QString mRootDir;      ///< Root directory containing generated files.
  QString mCurrentFile;  ///< Path of the currently opened file.

  QFileSystemModel* mModel = nullptr;    ///< File system model backing the tree view.
  FileTypeProxyModel* mProxy = nullptr;  ///< Proxy model used for filtering/sorting file types.
  // QTreeView* mTree = nullptr;

  QPlainTextEdit* mEditor = nullptr;  ///< Text editor used to display and edit file contents.

  QAction* mSaveAction = nullptr;          ///< Action for saving the current file.
  QAction* mRevertAction = nullptr;        ///< Action for reverting changes to the current file.
  QAction* mOpenExternalAction = nullptr;  ///< Action for opening the file in an external editor.
};