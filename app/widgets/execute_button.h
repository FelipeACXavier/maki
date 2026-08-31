#pragma once

#include "dropdown_button.h"

class ExecuteButton : public DropDownButton
{
  Q_OBJECT

public:
  ExecuteButton(QWidget* parent = nullptr);

  void addOption(const QString& name, const QString& id);
  void removeOption(const QString& id);

  void setRunning(bool running);

  void reset() override;

signals:
  void executeRequested(const QString& option);
  void editOptionRequested(const QString& option);
  void renameOptionRequested(const QString& option);
  void deleteOptionRequested(const QString& option);

protected:
  QSize sizeHint() const override;
  // void paintEvent(QPaintEvent* event) override;
  int leadingContentWidth(const oclero::qlementine::Theme& theme) const override;
  void paintLeadingContent(QPainter& painter, const QRect& rect, const oclero::qlementine::Theme& theme) override;

private:
  struct OptionInfo
  {
    QString id;
    QString name;
  };

  OptionInfo mCurrentOption;
  bool mRunning = false;
  const QString DEFAULT_TEXT;
  const QString ADD_NEW_TEXT;

  void setCurrentOption(const OptionInfo& option);
  OptionInfo currentOption();

  void buildMenu(QMenu* menu, const OptionInfo& option, bool addSelect);

  void rebuildMenu(const std::vector<OptionInfo>& pipelines);
  void setupDone();

  void updateButtonText();
  void showContextMenu(const QPoint& point);

  std::vector<OptionInfo> getPipelineNames() const;
};
