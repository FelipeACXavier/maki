#include "save_handler.h"

#include <QBuffer>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsItem>

#include "elements/connection.h"
#include "elements/connector.h"
#include "elements/node.h"
#include "logging.h"

SaveHandler::SaveHandler(QWidget* parent)
    : QWidget(parent)
    , mLastDir(QDir::homePath())
    , mCurrentFile("")
{
}

void SaveHandler::save(const QList<QGraphicsItem*>& items)
{
  if (mCurrentFile.isEmpty())
  {
    saveFileAs(items);
  }
  else
    saveToFile(items);
}

void SaveHandler::saveFileAs(const QList<QGraphicsItem*>& items)
{
  auto fileName = QFileDialog::getSaveFileName(this,
                                               tr("Save File"), mLastDir,
                                               tr("All Files (*);;Low-Code platform (*.lcp)"));

  if (fileName.isEmpty())
    return;

  storeFilename(fileName);
  saveToFile(items);
}

void SaveHandler::saveToFile(const QList<QGraphicsItem*>& items)
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

  QFile file(mCurrentFile);
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

Result<SaveInfo> SaveHandler::load()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open File"), mLastDir,
                                                  tr("All Files (*);;Low-Code platform (*.lcp)"));

  storeFilename(fileName);

  SaveInfo info;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return Result<SaveInfo>::Failed("Failed to open file for reading: " + file.errorString().toStdString());

  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_6_0);

  in >> info;

  file.close();

  return info;
}

void SaveHandler::storeFilename(const QString& fileName)
{
  if (fileName.isEmpty())
    return;

  mCurrentFile = fileName;

  QFileInfo fileInfo(fileName);
  mLastDir = fileInfo.absolutePath();  // Update the last directory to the current one
}
