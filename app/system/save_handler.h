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

  void save(const QList<QGraphicsItem*>& items);
  void saveToFile(const QList<QGraphicsItem*>& items);
  void saveFileAs(const QList<QGraphicsItem*>& items);

  Result<SaveInfo> load();

private:
  QString mLastDir;
  QString mCurrentFile;

  void storeFilename(const QString& fileName);
};
