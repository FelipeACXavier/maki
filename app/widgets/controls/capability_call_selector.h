#pragma once

#include <QString>
#include <QWidget>

#include "control_widget.h"

class StyledFrame;
class LibraryContainer;
class NodeSaveInfo;

class CapabilityCallSelector : public maki::ControlWidget
{
  Q_OBJECT

public:
  static CapabilityCallSelector* create(QWidget* parent = nullptr);
  CapabilityCallSelector(QWidget* parent = nullptr);

  void start() override;
  void buildLibraries(const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);

signals:
  void accepted(const QStringList& selected);

protected:
  void wheelEvent(QWheelEvent*) override;

private:
  StyledFrame* mContent;
  QString libraryFromId(const QString& id) const;
  LibraryContainer* getLibrary(const QString& id);
};
