#include "save_handler.h"

#include <QBuffer>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QJsonDocument>

#include "canvas.h"
#include "elements/node.h"
#include "logging.h"
#include "main_window.h"

SaveHandler::SaveHandler(QWidget* parent)
    : QObject()
    , mLastDir(QDir::homePath())
    , mCurrentFile("")
    , mParentWidget(parent)
{
}

void SaveHandler::newFileCreated()
{
  mCurrentFile.clear();  
}

VoidResult SaveHandler::save(Canvas* canvas)
{
  if (mCurrentFile.isEmpty() || mCurrentFile.isNull())
    return saveFileAs(canvas);

  return saveToFile(canvas);
}

VoidResult SaveHandler::saveFileAs(Canvas* canvas)
{
  QString fileName = openAtCenter(Function::SAVE);

  if (fileName.isEmpty())
    return VoidResult::Failed("File not set");

  storeFilename(fileName);
  return saveToFile(canvas);
}

VoidResult SaveHandler::saveToFile(Canvas* canvas)
{
  SaveInfo info;
  info.canvasInfo.scale = canvas->getScale();
  info.canvasInfo.center = canvas->getCenter();

  for (const auto& item : canvas->items())
  {
    if (item->type() == NodeItem::Type)
    {
      auto node = static_cast<NodeItem*>(item);
      if (node->parentNode() != nullptr)
        continue;

      if (node->function() == Types::LibraryTypes::STRUCTURAL)
        info.structuralNodes.push_back(std::make_shared<NodeSaveInfo>(node->saveInfo()));
      else
        info.behaviouralNodes.push_back(std::make_shared<NodeSaveInfo>(node->saveInfo()));
    }
  }

  QFile file(mCurrentFile);
  if (!file.open(QIODevice::WriteOnly))
    return VoidResult::Failed("Could not open file for writing: " + file.errorString().toStdString());

  QFileInfo fileInfo(mCurrentFile);
  QString extension = fileInfo.suffix();
  if (fileInfo.suffix() == "json")
  {
    QJsonDocument document(info.toJson());
    file.write(document.toJson());
  }
  else
  {
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    out << info;
  }

  file.flush();
  file.close();

  return VoidResult();
}

Result<SaveInfo> SaveHandler::load()
{
  QString fileName = openAtCenter(Function::LOAD);

  if (fileName.isEmpty())
    return Result<SaveInfo>::Failed("Not loading diagram");

  storeFilename(fileName);

  SaveInfo info;
  QFileInfo fileInfo(fileName);
  QString extension = fileInfo.suffix();
  if (fileInfo.suffix() == "json")
  {
    auto saveFile = JSON::fromFile(fileName);
    if (!saveFile.IsSuccess())
      return Result<SaveInfo>::Failed("Failed to open file for reading: " + saveFile.ErrorMessage());

    auto fileContents = saveFile.Value();
    info = SaveInfo::fromJson(fileContents);
  }
  else
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
      return Result<SaveInfo>::Failed("Failed to open file for reading: " + file.errorString().toStdString());

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    in >> info;
    file.close();
  }

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

QString SaveHandler::openAtCenter(Function function)
{
  QFileDialog dialog(mParentWidget);

  QRect parentGeometry = mParentWidget->geometry();

  // Calculate the center of the parent (main window)
  int x = parentGeometry.left() + (parentGeometry.width() - dialog.width()) / 2;
  int y = parentGeometry.top() + (parentGeometry.height() - dialog.height()) / 2;

  // Set the position of the dialog to the center of the main window
  dialog.move(x, y);

  if (function == Function::SAVE)
    return dialog.getSaveFileName(mParentWidget, tr("Save diagram"), mLastDir, tr("All Files (*);;Low-Code platform (*.lcp)"));

  return dialog.getOpenFileName(mParentWidget, tr("Open diagram"), mLastDir, tr("All Files (*);;Low-Code platform (*.lcp)"));
}
