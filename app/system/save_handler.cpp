#include "save_handler.h"

#include <QBuffer>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>

#include "canvas.h"
#include "elements/node.h"
#include "flow_info.h"
#include "keys.h"
#include "logging.h"
#include "main_window.h"
#include "node_info.h"
#include "result.h"
#include "widgets/dialogs/text_prompt.h"

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

QString SaveHandler::lastDir() const
{
  return mLastDir;
}

void SaveHandler::setLastDir(const QString& dir)
{
  if (dir.isEmpty())
    mLastDir = QDir::homePath();
  else
    mLastDir = dir;
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
  CanvasSaveInfo canvasInfo;
  canvasInfo.setScale(canvas->getScale());
  canvasInfo.setCenter(canvas->getCenter());

  SaveInfo info;
  info.setCanvasInfo(canvasInfo);

  for (const auto& item : canvas->items())
  {
    if (item->type() == NodeItem::Type)
    {
      auto node = static_cast<NodeItem*>(item);
      if (node->parentNode() != nullptr)
        continue;

      if (node->function() == Types::LibraryTypes::STRUCTURAL)
        info.addNode(std::make_shared<NodeSaveInfo>(node->saveInfo()));
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

  emit fileSaved(mCurrentFile);

  return VoidResult();
}

Result<SaveInfo> SaveHandler::load()
{
  QString fileName = openAtCenter(Function::LOAD);
  return load(fileName);
}

Result<SaveInfo> SaveHandler::load(const QString& fileName)
{
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

  emit fileLoaded(fileName);

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
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);

  switch (function)
  {
    case Function::SAVE:
      dialog.setAcceptMode(QFileDialog::AcceptSave);
      dialog.setFileMode(QFileDialog::AnyFile);
      dialog.setWindowTitle(tr("Save diagram"));
      dialog.setNameFilter(tr("MAKI diagram (*.json);;All Files (*)"));
      break;

    case Function::LOAD:
      dialog.setAcceptMode(QFileDialog::AcceptOpen);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setWindowTitle(tr("Open diagram"));
      dialog.setNameFilter(tr("MAKI diagram (*.json);;All Files (*)"));
      break;

    case Function::SAVE_PROJECT_DIR:
      dialog.setAcceptMode(QFileDialog::AcceptOpen);
      dialog.setFileMode(QFileDialog::Directory);
      dialog.setOption(QFileDialog::ShowDirsOnly, true);
      dialog.setWindowTitle(tr("Save MAKI project folder"));
      break;

    case Function::LOAD_PROJECT_DIR:
      dialog.setAcceptMode(QFileDialog::AcceptOpen);
      dialog.setFileMode(QFileDialog::Directory);
      dialog.setOption(QFileDialog::ShowDirsOnly, true);
      dialog.setWindowTitle(tr("Open MAKI project folder"));
      break;
  }

  dialog.setDirectory(mLastDir);

  if (dialog.exec() != QDialog::Accepted)
    return QString();

  const QString selected = dialog.selectedFiles().value(0);

  if (!selected.isEmpty())
    mLastDir = QFileInfo(selected).absolutePath();

  return selected;
}

VoidResult SaveHandler::saveProject(SaveInfo& project, bool override)
{
  if (project.name.trimmed().isEmpty() || override)
  {
    auto name = maki::textPrompt(tr("Project name"), tr("Choose a project name"), mParentWidget);

    if (name.isEmpty())
      return VoidResult::Failed("Project name is required.");

    project.name = name;
  }

  if (project.rootPath.isEmpty() || override)
  {
    QString parentPath = openAtCenter(Function::SAVE_PROJECT_DIR);
    if (parentPath.isEmpty())
      return VoidResult::Failed("No directory selected.");

    QDir parentDir(parentPath);
    auto projectPath = parentDir.filePath(project.name);
    if (!QFileInfo(projectPath).exists() && !parentDir.mkdir(project.name))
      return VoidResult::Failed("Could not create project directory: " + projectPath.toStdString());

    project.rootPath = projectPath;
  }

  return saveProjectInternal(project);
}

VoidResult SaveHandler::saveProjectAs(SaveInfo& project)
{
  return saveProject(project, true);
}

VoidResult SaveHandler::saveProjectInternal(const SaveInfo& project)
{
  if (project.rootPath.isEmpty())
    return VoidResult::Failed("Project root path is empty.");

  QDir root(project.rootPath);
  if (!root.exists())
    return VoidResult::Failed("No project directory: " + project.rootPath.toStdString());

  if (!root.mkpath("capabilities"))
    return VoidResult::Failed("Could not create capabilities directory.");

  if (!root.mkpath("flows"))
    return VoidResult::Failed("Could not create flows directory.");

  if (!root.mkpath("pipelines"))
    return VoidResult::Failed("Could not create pipelines directory.");

  auto manifestResult = saveManifest(project);
  if (!manifestResult.IsSuccess())
    return manifestResult;

  emit fileSaved(project.rootPath);

  return VoidResult();
}

VoidResult SaveHandler::writeJsonFile(const QString& path, const QJsonObject& object)
{
  QFileInfo fileInfo(path);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists())
    return VoidResult::Failed("Save directory does not exist: " + dir.absolutePath().toStdString());

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return VoidResult::Failed("Could not open file for writing: " + path.toStdString());

  QJsonDocument document(object);
  auto data = document.toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Saving %s", qPrintable(path));
  // LOG_DEBUG("%s", qPrintable(data));
  file.write(data);
  file.close();

  return VoidResult();
}

VoidResult SaveHandler::saveManifest(const SaveInfo& project)
{
  QJsonObject manifest;
  manifest["name"] = project.name;
  manifest["version"] = project.version;

  QJsonArray tasks;
  for (const auto& task : project.getnodes())
  {
    auto node = std::static_pointer_cast<NodeSaveInfo>(task);
    const auto name = node->getProperty("name").toString();
    auto toSave = node->toJson();

    QJsonArray capabilities;
    QJsonArray flows;

    for (const auto& c : task->getchildren())
    {
      auto capability = std::static_pointer_cast<NodeSaveInfo>(c);
      auto saved = saveCapability(name, project, *capability);
      if (!saved)
        return VoidResult::Failed(saved.ErrorMessage());

      capabilities.append(saved.Value());
    }

    for (const auto& f : task->getflows())
    {
      auto flow = std::static_pointer_cast<FlowSaveInfo>(f);
      auto saved = saveFlow(name, project, *flow);
      if (!saved)
        return VoidResult::Failed(saved.ErrorMessage());

      flows.append(saved.Value());
    }

    toSave[ConfigKeys::FLOWS] = flows;
    toSave[ConfigKeys::CHILDREN] = capabilities;
    tasks.append(toSave);
  }
  manifest["tasks"] = tasks;

  QJsonArray pipelines;
  for (const auto& pipeline : project.pipelines())
  {
    auto saved = savePipeline(project, *pipeline);
    if (!saved)
      return VoidResult::Failed(saved.ErrorMessage());

    pipelines.append(saved.Value());
  }
  manifest["pipelines"] = pipelines;

  const QString manifestPath = QDir(project.rootPath).filePath("manifest.json");
  return writeJsonFile(manifestPath, manifest);
}

Result<QString> SaveHandler::saveCapability(const QString& prefix, const SaveInfo& project, const NodeSaveInfo& capability)
{
  const QString fileName = QString("capabilities/%1-%2.json").arg(prefix, capability.getProperty("name").toString());
  const QString path = QDir(project.rootPath).filePath(fileName);

  auto wrote = writeJsonFile(path, capability.toJson());
  if (!wrote.IsSuccess())
    return Result<QString>::Failed(wrote.ErrorMessage());

  return fileName;
}

Result<QString> SaveHandler::saveFlow(const QString& prefix, const SaveInfo& project, const FlowSaveInfo& flow)
{
  const QString fileName = QString("flows/%1-%2.json").arg(prefix, flow.getname());
  const QString path = QDir(project.rootPath).filePath(fileName);
  auto wrote = writeJsonFile(path, flow.toJson());
  if (!wrote.IsSuccess())
    return Result<QString>::Failed(wrote.ErrorMessage());

  return fileName;
}

Result<QString> SaveHandler::savePipeline(const SaveInfo& project, const FlowSaveInfo& pipeline)
{
  const QString fileName = QString("pipelines/%1.json").arg(pipeline.getname());
  const QString path = QDir(project.rootPath).filePath(fileName);
  auto wrote = writeJsonFile(path, pipeline.toJson());
  if (!wrote.IsSuccess())
    return Result<QString>::Failed(wrote.ErrorMessage());

  return fileName;
}

QString SaveHandler::sanitizeFileName(QString name) const
{
  name.replace(QRegularExpression(R"([<>:"/\\|?*])"), "_");
  name.replace(" ", "_");
  return name;
}

Result<SaveInfo> SaveHandler::loadProject()
{
  const QString projectDir = openAtCenter(Function::LOAD_PROJECT_DIR);
  return loadProject(projectDir);
}

Result<SaveInfo> SaveHandler::loadProject(const QString& fileToLoad)
{
  const QString manifestPath = QDir(fileToLoad).filePath("manifest.json");

  if (!QFileInfo::exists(manifestPath))
    return Result<SaveInfo>::Failed("No manifest.json found in selected project folder.");

  return loadProjectManifest(manifestPath);
}

Result<SaveInfo> SaveHandler::loadProjectManifest(const QString& manifestPath)
{
  QFileInfo manifestInfo(manifestPath);

  if (!manifestInfo.exists())
    return Result<SaveInfo>::Failed("Project manifest not found: " + manifestPath.toStdString());

  const QString projectRoot = manifestInfo.absoluteDir().absolutePath();
  auto manifestFile = JSON::fromFile(manifestPath);
  if (!manifestFile.IsSuccess())
    return Result<SaveInfo>::Failed("Failed to open manifest: " + manifestFile.ErrorMessage());

  QJsonObject manifestJson = manifestFile.Value();

  SaveInfo project;
  project.rootPath = projectRoot;
  project.name = manifestJson.value(ConfigKeys::NAME).toString();
  project.version = manifestJson.value("version").toString();

  const QJsonArray tasks = manifestJson["tasks"].toArray();
  for (const QJsonValue& value : tasks)
  {
    const QJsonObject taskJson = value.toObject();

    NodeSaveInfo task = NodeSaveInfo::fromJson(taskJson);
    for (const QJsonValue& child : taskJson.value("children").toArray())
    {
      const QString absolutePath = QDir(projectRoot).filePath(child.toString());
      auto read = JSON::fromFile(absolutePath);
      if (!read.IsSuccess())
        return Result<SaveInfo>::Failed(read.ErrorMessage());

      QJsonObject capabilityJson = read.Value();
      task.addChild(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(capabilityJson)));
    }

    for (const QJsonValue& flow : taskJson.value("flows").toArray())
    {
      const QString absolutePath = QDir(projectRoot).filePath(flow.toString());
      auto read = JSON::fromFile(absolutePath);
      if (!read.IsSuccess())
        return Result<SaveInfo>::Failed(read.ErrorMessage());

      QJsonObject flowJson = read.Value();
      task.addFlow(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(flowJson)));
    }

    project.addNode(std::make_shared<NodeSaveInfo>(task));
  }

  const QJsonArray pipelines = manifestJson["pipelines"].toArray();
  for (const QJsonValue& value : pipelines)
  {
    const auto pipelineFile = value.toString();
    const auto absolutePath = QDir(projectRoot).filePath(pipelineFile);
    auto read = JSON::fromFile(absolutePath);
    if (!read.IsSuccess())
      return Result<SaveInfo>::Failed(read.ErrorMessage());

    QJsonObject pipelineJson = read.Value();
    project.addPipeline(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(pipelineJson)));
  }

  emit fileLoaded(project.rootPath);

  return project;
}