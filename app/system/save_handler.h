#pragma once

#include <QString>

#include "elements/save_info.h"
#include "result.h"

class QGraphicsItem;

class SaveHandler
{
public:
  SaveHandler();

  void setSaveFile(const QString& filename);
  void save(const QList<QGraphicsItem*>& items) const;
  Result<SaveInfo> load() const;

private:
  QString mSaveFile;
};
