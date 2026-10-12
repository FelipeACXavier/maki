
#pragma once

#include <QPointer>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <memory>

#include "control_widget.h"

class QComboBox;
class QLabel;
class QPushButton;
class QStackedWidget;
class QWheelEvent;
class QTextBrowser;

class StyledFrame;
class LibraryContainer;
class NodeSaveInfo;
class NodeItem;

namespace oclero::qlementine
{
class Label;
}

namespace maki
{
class SelectorWidget;
}

class CapabilityCallSelector : public maki::ControlWidget
{
  Q_OBJECT

public:
  static CapabilityCallSelector* create(NodeItem* node, QWidget* parent = nullptr);
  CapabilityCallSelector(NodeItem* node, QWidget* parent = nullptr);

  void start() override;
  void buildLibraries(const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);

signals:
  void accepted(const QStringList& selected);

protected:
  void wheelEvent(QWheelEvent* event) override;

private:
  void selectCapability(const QString& instanceId, const QString& capabilityType);
  void confirmSelection();
  void showCapabilityPage();

  QString libraryFromId(const QString& id) const;
  LibraryContainer* getLibrary(const QString& id);

  QStackedWidget* mStack = nullptr;
  StyledFrame* mContent = nullptr;

  oclero::qlementine::Label* mCapabilityLabel = nullptr;
  maki::SelectorWidget* mEventCombo = nullptr;
  QPushButton* mConfirmButton = nullptr;

  std::shared_ptr<NodeSaveInfo> mSelectedCapability;
  NodeItem* mNode;
  QTextBrowser* mEventInfo;
};
