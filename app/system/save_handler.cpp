#include "save_handler.h"

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsItem>

#include "elements/connection.h"
#include "elements/connector.h"
#include "elements/node.h"
#include "logging.h"

SaveHandler::SaveHandler()
{
}

void SaveHandler::setSaveFile(const QString& filename)
{
  mSaveFile = filename;
}

void SaveHandler::save(const QList<QGraphicsItem*>& items) const
{
  SaveInfo info;
  for (const auto& item : items)
  {
    if (item->type() == NodeItem::Type)
    {
      auto node = static_cast<NodeItem*>(item);
      if (node->function() == Types::LibraryTypes::STRUCTURAL)
        info.structuralNodes.push_front(node->saveInfo());
      else
        info.behaviouralNodes.push_back(node->saveInfo());

      for (const auto& connector : node->connectors())
        for (const auto& conn : connector->connectionsFromThis())
          info.connections.push_back(conn->saveInfo());
    }
  }

  QFile file(mSaveFile);
  if (!file.open(QIODevice::WriteOnly))
  {
    LOG_WARNING("Could not open file for writing: %s", qPrintable(file.errorString()));
    return;
  }

  QDataStream out(&file);
  out.setVersion(QDataStream::Qt_6_0);

  out << info;

  file.flush();
  file.close();
}

Result<SaveInfo> SaveHandler::load() const
{
  SaveInfo info;

  QFile file(mSaveFile);
  if (!file.open(QIODevice::ReadOnly))
    return Result<SaveInfo>::Failed("Failed to open file for reading: " + file.errorString().toStdString());

  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_6_0);

  in >> info.structuralNodes;
  in >> info.behaviouralNodes;
  in >> info.connections;

  file.close();

  return info;
}
