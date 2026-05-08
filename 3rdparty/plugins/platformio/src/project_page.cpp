#include "project_page.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace maki
{
ProjectPage::ProjectPage(QWidget* parent)
    : QWidget(parent)
{
  auto* root = new QVBoxLayout(this);

  auto* targetBox = new QGroupBox("Target");
  auto* form = new QFormLayout(targetBox);

  mBoardEdit = new QLineEdit("esp32dev");
  mPlatformEdit = new QLineEdit("espressif32");
  mFrameworkEdit = new QLineEdit("arduino");
  mEnvironmentEdit = new QLineEdit("esp32dev");

  mUploadPortCombo = new QComboBox();
  mUploadPortCombo->setEditable(true);

  mMonitorPortCombo = new QComboBox();
  mMonitorPortCombo->setEditable(true);

  mMonitorSpeedEdit = new QLineEdit("115200");

  form->addRow("Board", mBoardEdit);
  form->addRow("Platform", mPlatformEdit);
  form->addRow("Framework", mFrameworkEdit);
  form->addRow("Environment", mEnvironmentEdit);
  form->addRow("Upload port", mUploadPortCombo);
  form->addRow("Monitor port", mMonitorPortCombo);
  form->addRow("Monitor speed", mMonitorSpeedEdit);

  auto* buttonRow = new QWidget();
  auto* buttons = new QHBoxLayout(buttonRow);
  buttons->setContentsMargins(0, 0, 0, 0);

  auto* refreshButton = new QPushButton("Refresh devices");
  auto* generateButton = new QPushButton("Generate");
  auto* buildButton = new QPushButton("Build");
  auto* uploadButton = new QPushButton("Upload");
  auto* monitorButton = new QPushButton("Monitor");

  buttons->addWidget(refreshButton);
  buttons->addStretch();
  buttons->addWidget(generateButton);
  buttons->addWidget(buildButton);
  buttons->addWidget(uploadButton);
  buttons->addWidget(monitorButton);

  mOutput = new QPlainTextEdit();
  mOutput->setReadOnly(true);

  root->addWidget(targetBox);
  root->addWidget(buttonRow);
  root->addWidget(mOutput, 1);

  connect(refreshButton, &QPushButton::clicked, this, &ProjectPage::refreshDevicesRequested);
  connect(generateButton, &QPushButton::clicked, this, &ProjectPage::generateRequested);
  connect(buildButton, &QPushButton::clicked, this, &ProjectPage::buildRequested);
  connect(uploadButton, &QPushButton::clicked, this, &ProjectPage::uploadRequested);
  connect(monitorButton, &QPushButton::clicked, this, &ProjectPage::monitorRequested);
}

maki::PlatformIOTargetProfile ProjectPage::profile() const
{
  PlatformIOTargetProfile p;
  p.board = mBoardEdit->text().trimmed();
  p.platform = mPlatformEdit->text().trimmed();
  p.framework = mFrameworkEdit->text().trimmed();
  p.environment = mEnvironmentEdit->text().trimmed();

  p.uploadPort = mUploadPortCombo->currentText().trimmed();
  p.monitorPort = mMonitorPortCombo->currentText().trimmed();
  p.monitorSpeed = mMonitorSpeedEdit->text().toInt();

  if (p.environment.isEmpty())
    p.environment = p.board;

  return p;
}

void ProjectPage::setDevices(const QStringList& devices)
{
  const QString oldUpload = mUploadPortCombo->currentText();
  const QString oldMonitor = mMonitorPortCombo->currentText();

  mUploadPortCombo->clear();
  mMonitorPortCombo->clear();

  mUploadPortCombo->addItem("");
  mMonitorPortCombo->addItem("");

  for (const auto& device : devices)
  {
    mUploadPortCombo->addItem(device);
    mMonitorPortCombo->addItem(device);
  }

  mUploadPortCombo->setCurrentText(oldUpload);
  mMonitorPortCombo->setCurrentText(oldMonitor);
}

void ProjectPage::appendOutput(const QString& text)
{
  mOutput->appendPlainText(text.trimmed());
}
}  // namespace maki