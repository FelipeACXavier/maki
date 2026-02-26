// GeneratedFilesPanel.cpp
#include "file_menu.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "file_menu_proxy.h"
#include "logging.h"

GeneratedFilesPanel::GeneratedFilesPanel(QWidget* parent)
    : QTreeView(parent)
{
  mModel = new QFileSystemModel(this);
  mModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
  mModel->setRootPath(QCoreApplication::applicationDirPath());
  mModel->sort(0, Qt::SortOrder::AscendingOrder);

  mProxy = new FileTypeProxyModel(this);
  mProxy->setSourceModel(mModel);

  setModel(mProxy);
  // setModel(mModel);
  setHeaderHidden(false);
  setSortingEnabled(true);
  setUniformRowHeights(true);
  setAnimated(true);
  setVerticalScrollMode(ScrollMode::ScrollPerItem);
  setIndentation(12);

  header()->setSectionResizeMode(0, QHeaderView::Stretch);
  header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

  header()->setStretchLastSection(false);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Do not show the date column
  hideColumn(3);

  // Editor side
  // auto* editorContainer = new QWidget(splitter);
  // auto* editorLayout = new QVBoxLayout(editorContainer);
  // editorLayout->setContentsMargins(0, 0, 0, 0);
  // editorLayout->setSpacing(0);

  // auto* toolbar = new QToolBar(editorContainer);
  // toolbar->setIconSize(QSize(16, 16));

  // mSaveAction = toolbar->addAction("Save");
  // mSaveAction->setShortcut(QKeySequence::Save);

  // mRevertAction = toolbar->addAction("Revert");
  // mOpenExternalAction = toolbar->addAction("Open externally");

  mEditor = new QPlainTextEdit(this);
  mEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
  mEditor->hide();

  // editorLayout->addWidget(toolbar);
  // editorLayout->addWidget(mEditor);

  // Main layout
  // auto* mainLayout = new QVBoxLayout(this);
  // mainLayout->setContentsMargins(0, 0, 0, 0);
  // mainLayout->addWidget(splitter);

  // splitter->setStretchFactor(0, 1);
  // splitter->setStretchFactor(1, 2);

  // Behaviour: open on click
  connect(this, &QTreeView::clicked, this, [this](const QModelIndex& idx) {
    if (!idx.isValid())
      return;

    // Map proxy index → source index
    QModelIndex sourceIndex = mProxy->mapToSource(idx);

    if (!sourceIndex.isValid())
      return;

    if (mModel->isDir(sourceIndex))
      return;

    const QString path = mModel->filePath(sourceIndex);
    if (path == mCurrentFile)
      return;

    openPathInEditor(path);
  });
}

void GeneratedFilesPanel::setGenerationRoot(const QString& dir)
{
  mRootDir = QFileInfo(dir).absoluteFilePath();

  mModel->setRootPath(mRootDir);
  mModel->sort(0, Qt::SortOrder::AscendingOrder);

  // Source (filesystem) index
  const QModelIndex srcRootIdx = mModel->index(mRootDir);

  // Map to proxy index used by the view
  const QModelIndex proxyRootIdx = mProxy->mapFromSource(srcRootIdx);

  // Set root on the view (not on the model)
  setRootIndex(proxyRootIdx);
}

bool GeneratedFilesPanel::maybeSaveCurrent()
{
  if (mCurrentFile.isEmpty())
    return true;
  if (!mEditor->document()->isModified())
    return true;

  const auto r = QMessageBox::question(
      this,
      "Unsaved changes",
      "You have unsaved changes. Save them?",
      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
      QMessageBox::Yes);

  if (r == QMessageBox::Cancel)
    return false;
  if (r == QMessageBox::Yes)
    return saveCurrent();
  return true;  // No
}

bool GeneratedFilesPanel::saveCurrent()
{
  if (mCurrentFile.isEmpty())
    return false;

  // Guardrail: only allow saving inside generation root
  const QString canonicalRoot = QFileInfo(mRootDir).canonicalFilePath();
  const QString canonicalFile = QFileInfo(mCurrentFile).canonicalFilePath();
  if (!canonicalRoot.isEmpty() && !canonicalFile.startsWith(canonicalRoot))
  {
    QMessageBox::warning(this, "Save blocked", "Refusing to save outside the generation directory.");
    return false;
  }

  QSaveFile out(mCurrentFile);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QMessageBox::critical(this, "Save failed", out.errorString());
    return false;
  }

  const QByteArray bytes = mEditor->toPlainText().toUtf8();
  if (out.write(bytes) != bytes.size())
  {
    QMessageBox::critical(this, "Save failed", out.errorString());
    return false;
  }

  if (!out.commit())
  {
    QMessageBox::critical(this, "Save failed", out.errorString());
    return false;
  }

  mEditor->document()->setModified(false);
  emit fileSaved(mCurrentFile);
  return true;
}

void GeneratedFilesPanel::openPathInEditor(const QString& filePath)
{
  if (!isLikelyTextFile(filePath))
  {
    setCurrentFile(filePath);
    mEditor->setPlainText("[Not a text file / unsupported file type]");
    mEditor->setReadOnly(true);
    mEditor->document()->setModified(false);
    return;
  }

  // Optional: reject huge files
  const qint64 size = QFileInfo(filePath).size();
  const qint64 maxBytes = 2 * 1024 * 1024;  // 2 MB
  if (size > maxBytes)
  {
    setCurrentFile(filePath);
    mEditor->setPlainText(QString("[File too large to open here (%1 bytes)]").arg(size));
    mEditor->setReadOnly(true);
    mEditor->document()->setModified(false);
    return;
  }

  QString text, err;
  if (!loadFileUtf8(filePath, text, &err))
  {
    QMessageBox::critical(this, "Open failed", err);
    return;
  }

  setCurrentFile(filePath);
  mEditor->setReadOnly(true);
  mEditor->setPlainText(text);
  mEditor->document()->setModified(false);

  LOG_DEBUG("Opening editor for file: %s", qPrintable(filePath));
  emit openExternallyRequested(mEditor);
}

void GeneratedFilesPanel::setCurrentFile(const QString& filePath)
{
  mCurrentFile = filePath;
}

bool GeneratedFilesPanel::isLikelyTextFile(const QString& filePath) const
{
  // Simple heuristic (you can expand this)
  const QString ext = QFileInfo(filePath).suffix().toLower();
  static const QSet<QString> textExt{
      "txt", "md", "json", "yaml", "yml", "xml", "ini", "cfg", "conf",
      "cpp", "cc", "c", "h", "hpp", "hh", "qml", "js", "ts", "py", "sh",
      "kd", "dzn"};

  if (textExt.contains(ext))
    return true;

  // also allow “no extension” small files (often config)
  if (ext.isEmpty())
    return true;

  return false;
}

bool GeneratedFilesPanel::loadFileUtf8(const QString& filePath, QString& outText, QString* outError) const
{
  QFile f(filePath);
  if (!f.open(QIODevice::ReadOnly))
  {
    if (outError)
      *outError = f.errorString();
    return false;
  }

  const QByteArray bytes = f.readAll();

  // Basic binary check: contains NUL
  if (bytes.contains('\0'))
  {
    if (outError)
      *outError = "File looks binary (contains NUL bytes).";
    return false;
  }

  outText = QString::fromUtf8(bytes);
  return true;
}