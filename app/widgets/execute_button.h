#pragma once

#include "dropdown_button.h"

class ExecuteButton : public DropDownButton
{
  Q_OBJECT

public:
  ExecuteButton(QWidget* parent = nullptr);

  QString currentOption();
  void addOption(const QString& name);
  void removeOption(const QString& name);
  void setCurrentOption(const QString& name);

  void setRunning(bool running);

  QStringList getPipelineNames() const;

signals:
  void executeRequested(const QString& option);
  void editOptionRequested(const QString& option);
  void deleteOptionRequested(const QString& option);

protected:
  QSize sizeHint() const override;
  // void paintEvent(QPaintEvent* event) override;
  int leadingContentWidth(const oclero::qlementine::Theme& theme) const override;
  void paintLeadingContent(QPainter& painter, const QRect& rect, const oclero::qlementine::Theme& theme) override;

private:
  QString mCurrentOption;
  bool mRunning = false;
  const QString DEFAULT_TEXT;
  const QString ADD_NEW_TEXT;

  void buildMenu(QMenu* menu, const QString& option, bool addSelect);
  void rebuildMenu(const QStringList& pipelines);
  void setupDone();

  void updateButtonText();
  void showContextMenu(const QPoint& point);
};
