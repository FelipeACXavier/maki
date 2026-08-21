#include "save_handler.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QBuffer>
#include <QDirIterator>
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
#include "type_helpers.h"
#include "type_registry.h"

VoidResult zipFolder(const QString& sourceDir, const QString& outputFile)
{
  QuaZip zip(outputFile);
  if (!zip.open(QuaZip::mdCreate))
    return VoidResult::Failed("Could not create archive: {}", outputFile.toStdString());

  QDir base(sourceDir);
  QDirIterator it(sourceDir, QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext())
  {
    const QString filePath = it.next();
    const QString relativePath = base.relativeFilePath(filePath);

    QFile input(filePath);
    if (!input.open(QIODevice::ReadOnly))
      return VoidResult::Failed("Could not read file: {}", filePath.toStdString());

    QuaZipFile outFile(&zip);
    QuaZipNewInfo info(relativePath, filePath);

    if (!outFile.open(QIODevice::WriteOnly, info))
      return VoidResult::Failed("Could not add file: {}", relativePath.toStdString());

    outFile.write(input.readAll());
    outFile.close();
  }

  zip.close();

  if (zip.getZipError() != UNZ_OK)
    return VoidResult::Failed("Failed to finalise archive");

  return VoidResult();
}

Result<QByteArray> zipFolderToBytes(const QString& sourceDir)
{
  QByteArray zipData;
  QBuffer buffer(&zipData);

  if (!buffer.open(QIODevice::WriteOnly))
    return Result<QByteArray>::Failed("Could not open zip buffer");

  QuaZip zip(&buffer);

  if (!zip.open(QuaZip::mdCreate))
    return Result<QByteArray>::Failed("Could not create archive");

  QDir base(sourceDir);
  QDirIterator it(sourceDir, QDir::Files, QDirIterator::Subdirectories);

  while (it.hasNext())
  {
    const QString filePath = it.next();
    const QString relativePath = base.relativeFilePath(filePath);

    QFile input(filePath);
    if (!input.open(QIODevice::ReadOnly))
      return Result<QByteArray>::Failed("Could not read file: {}", filePath.toStdString());

    QuaZipFile outFile(&zip);
    QuaZipNewInfo info(relativePath);

    if (!outFile.open(QIODevice::WriteOnly, info))
      return Result<QByteArray>::Failed("Could not add file: {}", relativePath.toStdString());

    outFile.write(input.readAll());
    outFile.close();
  }

  zip.close();

  if (zip.getZipError() != UNZ_OK)
    return Result<QByteArray>::Failed("Failed to finalise archive");

  return zipData;
}

VoidResult unzipProject(const QString& makiFile, const QString& outputDir)
{
  QuaZip zip(makiFile);
  if (!zip.open(QuaZip::mdUnzip))
    return VoidResult::Failed("Could not open archive");

  QDir().mkpath(outputDir);

  for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
  {
    const QString name = zip.getCurrentFileName();

    QuaZipFile file(&zip);
    if (!file.open(QIODevice::ReadOnly))
      return VoidResult::Failed("Could not read archive entry: {}", name);

    const QString outPath = QDir(outputDir).filePath(name);
    QDir().mkpath(QFileInfo(outPath).absolutePath());

    QFile outFile(outPath);
    if (!outFile.open(QIODevice::WriteOnly))
      return VoidResult::Failed("Could not write file: {}", outPath);

    outFile.write(file.readAll());
    outFile.close();
    file.close();
  }

  zip.close();
  return VoidResult();
}

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
  dialog.setOption(QFileDialog::DontUseNativeDialog, false);

  switch (function)
  {
    case Function::SAVE:
      dialog.setAcceptMode(QFileDialog::AcceptSave);
      dialog.setFileMode(QFileDialog::AnyFile);
      dialog.setWindowTitle(tr("Save diagram"));
      dialog.setNameFilter(tr("MAKI (*.maki);;All Files (*)"));
      break;

    case Function::LOAD:
      dialog.setAcceptMode(QFileDialog::AcceptOpen);
      dialog.setFileMode(QFileDialog::ExistingFile);
      dialog.setWindowTitle(tr("Open diagram"));
      dialog.setNameFilter(tr("MAKI (*.maki);;All Files (*)"));
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
#ifdef __EMSCRIPTEN__
  project.rootPath = "/tmp/maki-project";
  QDir(project.rootPath).removeRecursively();
  QDir().mkpath(project.rootPath);
#else
  if (project.rootPath.isEmpty() || project.saveFile.isEmpty() || project.name.trimmed().isEmpty() || override)
  {
    QString saveFile = openAtCenter(Function::SAVE);
    if (saveFile.isEmpty())
      return VoidResult::Failed("No file selected.");

    QFileInfo fileInfo(saveFile);
    project.name = fileInfo.completeBaseName();
    project.saveFile = saveFile;
    project.rootPath = "/tmp/maki-project";
  }
#endif

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
  root.removeRecursively();
  if (!root.mkpath("."))
    return VoidResult::Failed("No project directory: {}", project.rootPath);

  if (!root.mkpath("capabilities"))
    return VoidResult::Failed("Could not create capabilities directory.");

  if (!root.mkpath("flows"))
    return VoidResult::Failed("Could not create flows directory.");

  if (!root.mkpath("pipelines"))
    return VoidResult::Failed("Could not create pipelines directory.");

  auto manifestResult = saveManifest(project);
  if (!manifestResult.IsSuccess())
    return manifestResult;

#ifdef __EMSCRIPTEN__
  auto zipped = zipFolderToBytes(project.rootPath);
  if (!zipped)
    return VoidResult::Failed(zipped.ErrorMessage());

  QFileDialog::saveFileContent(zipped.Value(), QString("%1.maki").arg(project.name));
#else
  auto zipped = zipFolder(project.rootPath, project.saveFile);
  if (!zipped.IsSuccess())
    return zipped;
#endif

  emit fileSaved(project.saveFile);

  return VoidResult();
}

VoidResult SaveHandler::writeJsonFile(const QString& path, const QJsonObject& object)
{
  QFileInfo fileInfo(path);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists())
    return VoidResult::Failed("Save directory does not exist: {}", dir.absolutePath());

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return VoidResult::Failed("Could not open file for writing: {}", path);

  QJsonDocument document(object);
  auto data = document.toJson(QJsonDocument::Indented);
  file.write(data);
  file.close();

  return VoidResult();
}

VoidResult SaveHandler::saveManifest(const SaveInfo& project)
{
  QJsonObject manifest;
  manifest[ConfigKeys::NAME] = project.name;
  manifest["version"] = project.version;
  manifest["projectSaveFile"] = project.saveFile;

  manifest["canvas"] = project.canvasInfo().toJson();

  QJsonArray tasks;
  for (const auto& task : project.getnodes())
  {
    auto node = std::static_pointer_cast<NodeSaveInfo>(task);
    QString savedTask;
    ASSIGN_OR_RETURN_ON_FAILURE(savedTask, saveNodeTree(project, *node, "tasks"));
    LOG_INFO("Adding task: {}", savedTask);
    tasks.append(savedTask);
  }
  manifest["tasks"] = tasks;

  QString savedTypes;
  ASSIGN_OR_RETURN_ON_FAILURE(savedTypes, saveTypes(project.rootPath));
  manifest["types"] = savedTypes;

  QString savedParameters;
  ASSIGN_OR_RETURN_ON_FAILURE(savedParameters, saveMissionParameters(project.rootPath, project.missionParameters()));
  manifest["parameters"] = savedParameters;

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

Result<QString> SaveHandler::saveNodeTree(const SaveInfo& project, const NodeSaveInfo& node, const QString& folder)
{
  QDir root(project.rootPath);

  const QString nodeName = sanitizeFileName(node.getProperty("name").toString());
  const QString nodeFolder = QDir(folder).filePath(nodeName);

  QDir().mkpath(root.filePath(nodeFolder));
  QDir().mkpath(root.filePath(QDir(nodeFolder).filePath("flows")));
  QDir().mkpath(root.filePath(QDir(nodeFolder).filePath("children")));

  QJsonObject toSave = node.toJson();
  QJsonArray flows;
  for (const auto& f : node.getflows())
  {
    auto flow = std::static_pointer_cast<FlowSaveInfo>(f);

    QString flowFile;
    ASSIGN_OR_RETURN_ON_FAILURE(flowFile, saveFlow(root, nodeFolder, *flow));
    // const QString flowFile = QDir(nodeFolder).filePath(QString("flows/%1.json").arg(sanitizeFileName(flow->getname())));
    // auto wrote = writeJsonFile(root.filePath(flowFile), flow->toJson());
    // if (!wrote)
    // return Result<QString>::Failed(wrote.ErrorMessage());

    flows.append(flowFile);
  }

  QJsonArray children;
  for (const auto& c : node.getchildren())
  {
    auto child = std::static_pointer_cast<NodeSaveInfo>(c);

    QString childNodeFile;
    ASSIGN_OR_RETURN_ON_FAILURE(childNodeFile, saveNodeTree(project, *child, QDir(nodeFolder).filePath("children")));
    LOG_INFO("Saving child: {}", childNodeFile);
    children.append(childNodeFile);
  }

  toSave[ConfigKeys::FLOWS] = flows;
  toSave[ConfigKeys::CHILDREN] = children;

  const QString nodeFile = QDir(nodeFolder).filePath("node.json");
  auto wrote = writeJsonFile(root.filePath(nodeFile), toSave);
  if (!wrote)
    return Result<QString>::Failed(wrote.ErrorMessage());

  return nodeFile;
}

Result<QString> SaveHandler::saveFlow(const QDir& root, const QString& nodeFolder, const FlowSaveInfo& flow)
{
  const QString filePath = QDir(nodeFolder).filePath(QString("flows/%1.json").arg(sanitizeFileName(flow.getname())));
  auto wrote = writeJsonFile(root.filePath(filePath), flow.toJson());
  if (!wrote.IsSuccess())
    return Result<QString>::Failed(wrote.ErrorMessage());

  return QDir(nodeFolder).relativeFilePath(filePath);
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

Result<QString> SaveHandler::saveTypes(const QDir& root)
{
  QJsonArray types;
  const auto& registry = maki::TypeRegistry::instance();
  for (const auto& task : registry.allTypes())
    // We don't need to save the built-in or the library types, maki takes care of those
    if (!registry.isBuiltin(*task) && !registry.isFromLibrary(task->name))
      types.append(maki::typeDefinitionToJson(*task));

  QJsonObject json;
  json["types"] = types;

  const QString filePath = "types.json";
  auto wrote = writeJsonFile(root.filePath(filePath), json);

  return filePath;
}

Result<QString> SaveHandler::saveMissionParameters(const QDir& root, const QVector<maki::MissionParameter>& parameters)
{
  QJsonArray types;
  for (const auto& parameter : parameters)
    types.append(parameter.toJson());

  QJsonObject json;
  json["parameters"] = types;

  const QString filePath = "parameters.json";
  auto wrote = writeJsonFile(root.filePath(filePath), json);

  return filePath;
}

QString SaveHandler::sanitizeFileName(QString name) const
{
  name.replace(QRegularExpression(R"([<>:"/\\|?*])"), "_");
  name.replace(" ", "_");
  return name;
}

VoidResult SaveHandler::loadProject()
{
  QFileDialog::getOpenFileContent("MAKI projects (*.maki)", [this](const QString& fileName, const QByteArray& content) {
    if (fileName.isEmpty())
    {
      emit fileLoaded("", SaveInfo(), "No file selected.");
      return;
    }

    const QString tempFile = "/tmp/uploaded.maki";
    QFile file(tempFile);
    if (!file.open(QIODevice::WriteOnly))
    {
      emit fileLoaded("", SaveInfo(), "No file selected.");
      return;
    }

    file.write(content);
    file.close();

    auto loaded = loadProject(tempFile);
    if (!loaded.IsSuccess())
      emit fileLoaded(fileName, SaveInfo(), QString::fromStdString(loaded.ErrorMessage()));
    else
    {
      auto info = loaded.Value();
      emit fileLoaded(fileName, info, QString());
    }
  });

  return VoidResult();
}

Result<SaveInfo> SaveHandler::loadProject(const QString& fileToLoad)
{
  const auto projectDir = "/tmp/maki-project";
  auto unzipped = unzipProject(fileToLoad, projectDir);
  if (!unzipped)
    return Result<SaveInfo>::Failed(unzipped.ErrorMessage());

  const QString manifestPath = QDir(projectDir).filePath("manifest.json");

  if (!QFileInfo::exists(manifestPath))
    return Result<SaveInfo>::Failed("No manifest.json found in selected project folder.");

  return loadProjectManifest(manifestPath);
}

Result<SaveInfo> SaveHandler::loadProjectManifest(const QString& manifestPath)
{
  QFileInfo manifestInfo(manifestPath);

  if (!manifestInfo.exists())
    return Result<SaveInfo>::Failed("Project manifest not found: '{}'", manifestPath);

  const QString projectRoot = manifestInfo.absoluteDir().absolutePath();
  auto manifestFile = JSON::fromFile(manifestPath);
  if (!manifestFile.IsSuccess())
    return Result<SaveInfo>::Failed("Failed to open manifest: {}", manifestFile.ErrorMessage());

  QJsonObject manifestJson = manifestFile.Value();

  SaveInfo project;
  project.rootPath = projectRoot;
  if (manifestJson.contains(ConfigKeys::NAME))
    project.name = manifestJson.value(ConfigKeys::NAME).toString();
  if (manifestJson.contains("version"))
    project.version = manifestJson.value("version").toString();
  if (manifestJson.contains("projectSaveFile"))
    project.saveFile = manifestJson.value("projectSaveFile").toString();
  if (manifestJson.contains("canvas"))
    project.setCanvasInfo(CanvasSaveInfo::fromJson(manifestJson.value("canvas").toObject()));

  const QJsonArray tasks = manifestJson["tasks"].toArray();
  for (const QJsonValue& value : tasks)
  {
    if (!value.isString())
      return Result<SaveInfo>::Failed("Invalid task entry in manifest.");

    std::shared_ptr<NodeSaveInfo> loadedTask;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(loadedTask, loadNodeTree(projectRoot, value.toString()), SaveInfo);
    project.addNode(loadedTask);
  }

  RETURN_ON_FAILURE_AS(loadTypes(projectRoot, manifestJson), SaveInfo);
  RETURN_ON_FAILURE_AS(loadMissionParameters(projectRoot, manifestJson, project), SaveInfo);

  const QJsonArray pipelines = manifestJson["pipelines"].toArray();
  for (const QJsonValue& value : pipelines)
  {
    const auto pipelineFile = value.toString();
    const auto absolutePath = QDir(projectRoot).filePath(pipelineFile);

    JSON contents;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(contents, JSON::fromFile(absolutePath), SaveInfo);
    project.addPipeline(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(contents)));
  }

  return project;
}

VoidResult SaveHandler::loadTypes(const QString& projectRoot, const QJsonObject& manifestJson)
{
  if (!manifestJson.contains("types"))
    return VoidResult();

  if (!manifestJson["types"].isString())
    return VoidResult::Failed("Types entry in manifest has the wrong format");

  const QString absolutePath = QDir(projectRoot).filePath(manifestJson["types"].toString());
  auto read = JSON::fromFile(absolutePath);
  if (!read)
    return VoidResult::Failed(read.ErrorMessage());

  const QJsonObject json = read.Value();
  if (!json.contains("types") || !json["types"].isArray())
    return VoidResult::Failed("Invalid type file");

  for (const QJsonValue& value : json["types"].toArray())
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid type entry");

    auto definition = maki::typeDefinitionFromJson(value.toObject());
    if (!definition.IsSuccess())
      return VoidResult::Failed(definition.ErrorMessage());

    auto added = maki::TypeRegistry::instance().add(definition.Value());
    if (!added)
      return VoidResult::Failed(added.ErrorMessage());
  }

  return VoidResult();
}

VoidResult SaveHandler::loadMissionParameters(const QString& projectRoot, const QJsonObject& manifestJson, SaveInfo& project)
{
  if (!manifestJson.contains("parameters"))
    return VoidResult();

  if (!manifestJson["parameters"].isString())
    return VoidResult::Failed("Parameters entry in manifest has the wrong format");

  const QString absolutePath = QDir(projectRoot).filePath(manifestJson["parameters"].toString());
  auto read = JSON::fromFile(absolutePath);
  if (!read)
    return VoidResult::Failed(read.ErrorMessage());

  const QJsonObject json = read.Value();
  if (!json.contains("parameters") || !json["parameters"].isArray())
    return VoidResult::Failed("Invalid parameter file format");

  for (const QJsonValue& value : json["parameters"].toArray())
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid parameter entry");

    auto parameter = maki::MissionParameter::fromJson(value.toObject());
    if (!parameter.IsSuccess())
      return VoidResult::Failed(parameter.ErrorMessage());

    project.addParameter(parameter.Value());
  }

  return VoidResult();
}

Result<std::shared_ptr<NodeSaveInfo>> SaveHandler::loadNodeTree(const QString& projectRoot, const QString& nodeFile)
{
  const QString absolutePath = QDir(projectRoot).filePath(nodeFile);
  LOG_TRACE("loadNodeTree - projectRoot: {} nodeFile: {} absolutePath: {}", projectRoot, nodeFile, absolutePath);

  auto read = JSON::fromFile(absolutePath);
  if (!read)
    return Result<std::shared_ptr<NodeSaveInfo>>::Failed(read.ErrorMessage());

  const QJsonObject json = read.Value();
  auto node = std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(json));

  const QFileInfo nodeInfo(absolutePath);
  const QDir nodeDir = nodeInfo.absoluteDir();

  for (const auto& child : json.value(ConfigKeys::CHILDREN).toArray())
  {
    const QString childFile = QDir(projectRoot).absoluteFilePath(child.toString());
    std::shared_ptr<NodeSaveInfo> info;
    ASSIGN_OR_RETURN_ON_FAILURE(info, loadNodeTree(projectRoot, childFile));
    node->addChild(info);
  }

  for (const auto& flow : json.value(ConfigKeys::FLOWS).toArray())
  {
    const QString flowPath = QDir(projectRoot).absoluteFilePath(flow.toString());
    JSON contents;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(contents, JSON::fromFile(flowPath), std::shared_ptr<NodeSaveInfo>);
    node->addFlow(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(contents)));
  }

  return node;
}
