#pragma once

#include <QString>
#include <QWidget>

#include "control_widget.h"

class StyledFrame;
class LibraryContainer;

class CapabilitySelector : public maki::ControlWidget
{
  Q_OBJECT

public:
  static CapabilitySelector* create(QWidget* parent = nullptr);
  CapabilitySelector(QWidget* parent = nullptr);

  void start() override;

signals:
  void accepted(const QStringList& selected);

protected:
  void wheelEvent(QWheelEvent*) override;

private:
  StyledFrame* mContent;
  void buildLibraries();
  QString libraryFromId(const QString& id) const;
  LibraryContainer* getLibrary(const QString& id);
};