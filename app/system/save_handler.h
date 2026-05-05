#pragma once

#include <QString>
#include <QWidget>

#include "result.h"
#include "save_info.h"

class QGraphicsItem;
class Canvas;

/**
 * @brief Handles saving and loading of diagrams.
 */
class SaveHandler : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new SaveHandler object.
   *
   * @param parent The parent widget.
   */
  SaveHandler(QWidget* parent);

  /**
   * @brief Saves the current canvas to a file.
   *
   * @param canvas The canvas to save.
   * @return VoidResult The result of the operation.
   */
  VoidResult save(Canvas* canvas);

  /**
   * @brief Saves the current canvas to a new file.
   *
   * @param canvas The canvas to save.
   * @return VoidResult The result of the operation.
   */
  VoidResult saveToFile(Canvas* canvas);

  /**
   * @brief Saves the current canvas as a new file.
   *
   * @param canvas The canvas to save.
   * @return VoidResult The result of the operation.
   */
  VoidResult saveFileAs(Canvas* canvas);

  /**
   * @brief Gets the last directory used for saving or loading files.
   *
   * @return QString The last directory.
   */
  QString lastDir() const;

  /**
   * @brief Sets the last directory used for saving or loading files.
   *
   * @param dir The new last directory.
   */
  void setLastDir(const QString& dir);

  /**
   * @brief Handles a new file being created.
   */
  void newFileCreated();

  /**
   * @brief Loads a diagram from a file.
   *
   * @return Result<SaveInfo> The result of the operation.
   */
  Result<SaveInfo> load();

  /**
   * @brief Loads a diagram from a specific file.
   *
   * @param fileToLoad The file to load.
   * @return Result<SaveInfo> The result of the operation.
   */
  Result<SaveInfo> load(const QString& fileToLoad);

  /**
   * @brief Enumeration containing the accepted functionalities of the SaveHandler
   */
  enum class Function
  {
    SAVE,
    LOAD
  };

signals:
  /**
   * @brief Emitted when a file is loaded.
   *
   * @param file The name of the loaded file.
   */
  void fileLoaded(const QString& file);

  /**
   * @brief Emitted when a file is saved.
   *
   * @param file The name of the saved file.
   */
  void fileSaved(const QString& file);

private:
  QString mLastDir;      /// Last directory used for saving or loading files.
  QString mCurrentFile;  /// Current file being edited.

  QWidget* mParentWidget;  /// Parent widget.

  /**
   * @brief Opens a file dialog to select a file.
   *
   * @param function The type of operation (save or load).
   * @return QString The selected file name, or an empty string if the dialog was canceled.
   */
  QString openAtCenter(Function save);

  /**
   * @brief Stores the filename and updates the last directory.
   *
   * @param fileName The new file name.
   */
  void storeFilename(const QString& fileName);
};
