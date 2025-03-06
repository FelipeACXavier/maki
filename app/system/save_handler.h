#pragma once

#include <QString>
#include <QWidget>

#include "elements/save_info.h"
#include "result.h"

class QGraphicsItem;

class SaveHandler : public QWidget
{
  Q_OBJECT
public:
  SaveHandler(QWidget* parent = nullptr);

  VoidResult save(const QList<QGraphicsItem*>& items);
  VoidResult saveToFile(const QList<QGraphicsItem*>& items);
  VoidResult saveFileAs(const QList<QGraphicsItem*>& items);

  Result<SaveInfo> load();

  enum class Function
  {
    SAVE,
    LOAD
  };

private:
  QString mLastDir;
  QString mCurrentFile;

  QString openAtCenter(Function save);

  void storeFilename(const QString& fileName);
};
