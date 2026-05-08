#pragma once

#include <QWidget>

#include "target_profile.h"

class QLineEdit;
class QComboBox;
class QPlainTextEdit;

namespace maki
{
class ProjectPage final : public QWidget
{
  Q_OBJECT
public:
  explicit ProjectPage(QWidget* parent = nullptr);

  PlatformIOTargetProfile profile() const;
  void setDevices(const QStringList& devices);
  void appendOutput(const QString& text);

signals:
  void refreshDevicesRequested();
  void generateRequested();
  void buildRequested();
  void uploadRequested();
  void monitorRequested();

private:
  QLineEdit* mBoardEdit = nullptr;
  QLineEdit* mPlatformEdit = nullptr;
  QLineEdit* mFrameworkEdit = nullptr;
  QLineEdit* mEnvironmentEdit = nullptr;
  QComboBox* mUploadPortCombo = nullptr;
  QComboBox* mMonitorPortCombo = nullptr;
  QLineEdit* mMonitorSpeedEdit = nullptr;
  QPlainTextEdit* mOutput = nullptr;
};
}  // namespace maki