#pragma once

#include "dropdown_button.h"
#include "document/document_event.h"
#include "document/event_dispatcher.h"

class ExecuteButton : public DropDownButton
{
  Q_OBJECT

public:
  struct OptionInfo
  {
    QString id;
    QString name;
  };

  explicit ExecuteButton(QWidget* parent = nullptr);

  void start();
  void addOption(const QString& name, const QString& id);
  void removeOption(const QString& id);

  OptionInfo currentOption();

  void setRunning(bool running);

  void reset() override;

signals:
  void executeRequested(const QString& option, bool fromScratch);
  void editOptionRequested(const QString& option);
  void renameOptionRequested(const QString& option);
  void deleteOptionRequested(const QString& option);

protected:
  QSize sizeHint() const override;
  // void paintEvent(QPaintEvent* event) override;
  int leadingContentWidth(const oclero::qlementine::Theme& theme) const override;
  void paintLeadingContent(QPainter& painter, const QRect& rect, const oclero::qlementine::Theme& theme) override;

private:
  OptionInfo mCurrentOption;
  bool mRunning = false;
  EventDispatcher<DocumentEvent>::Subscription mDocumentSubscription;
  const QString DEFAULT_TEXT;
  const QString ADD_NEW_TEXT;

  void buildMenu(QMenu* menu, const OptionInfo& option, bool addSelect);

  void rebuildMenu(const std::vector<OptionInfo>& pipelines);
  void setupDone();

  void setCurrentOption(const OptionInfo& option);
  void updateButtonText();
  void showContextMenu(const QPoint& point);

  std::vector<OptionInfo> getPipelineNames() const;
};
