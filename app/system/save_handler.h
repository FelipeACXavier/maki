#pragma once

#include <QString>
#include <QWidget>

#include "result.h"
#include "save_info.h"

class QGraphicsItem;
class Canvas;

class SaveHandler : public QObject
{
  Q_OBJECT
public:
  SaveHandler(QWidget* parent);

  VoidResult save(Canvas* canvas);
  VoidResult saveToFile(Canvas* canvas);
  VoidResult saveFileAs(Canvas* canvas);

  QString lastDir() const;
  void setLastDir(const QString& dir);
  void newFileCreated();
  Result<SaveInfo> load();
  Result<SaveInfo> load(const QString& fileToLoad);

  enum class Function
  {
    SAVE,
    LOAD
  };

signals:
  void fileLoaded(const QString& file);
  void fileSaved(const QString& file);

private:
  QString mLastDir;
  QString mCurrentFile;

  QWidget* mParentWidget;

  QString openAtCenter(Function save);

  void storeFilename(const QString& fileName);
};
