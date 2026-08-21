#pragma once

#include <QDir>
#include <QString>
#include <QWidget>

#include "node_info.h"
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

  VoidResult saveProject(SaveInfo& project, bool override = false);
  VoidResult saveProjectAs(SaveInfo& project);

  VoidResult loadProject();
  Result<SaveInfo> loadProject(const QString& fileToLoad);
  Result<SaveInfo> loadProjectManifest(const QString& manifestPath);

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
   * @brief Enumeration containing the accepted functionalities of the SaveHandler
   */
  enum class Function
  {
    SAVE,
    LOAD,
  };

signals:
  /**
   * @brief Emitted when a file is loaded.
   *
   * @param file The name of the loaded file.
   */
  void fileLoaded(const QString& file, const SaveInfo& info, const QString& error);

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

  QString sanitizeFileName(QString name) const;

  VoidResult writeJsonFile(const QString& path, const QJsonObject& object);
  VoidResult saveProjectInternal(const SaveInfo& project);
  VoidResult saveManifest(const SaveInfo& project);

  Result<QString> saveNodeTree(const SaveInfo& project, const NodeSaveInfo& node, const QString& folder);
  Result<QString> saveFlow(const QDir& root, const QString& nodeFolder, const FlowSaveInfo& task);
  Result<QString> savePipeline(const SaveInfo& project, const FlowSaveInfo& task);
  Result<QString> saveTypes(const QDir& root);
  Result<QString> saveMissionParameters(const QDir& root, const QVector<maki::MissionParameter>& parameters);

  VoidResult loadTypes(const QString& projectRoot, const QJsonObject& manifestJson);
  VoidResult loadMissionParameters(const QString& projectRoot, const QJsonObject& manifestJson, SaveInfo& project);
  Result<std::shared_ptr<NodeSaveInfo>> loadNodeTree(const QString& projectRoot, const QString& nodeFile);
};
