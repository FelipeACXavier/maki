#pragma once
#include <QString>
#include <QWidget>

#include "control_widget.h"

class IDocument;

namespace maki
{
class IPipeline;
class ISettings;
class IUI;
class ILogging;

struct SimulationProperties
{
  maki::ControlWidget* widget;
  QColor highlight = QColor();
};

class IHostServices
{
public:
  virtual ~IHostServices() = default;

  virtual IDocument* document() const = 0;
  virtual ISettings* settings() const = 0;
  virtual IPipeline* pipeline() const = 0;
  virtual QString projectRoot() const = 0;
  virtual IUI* ui() const = 0;
  virtual ILogging* logger() const = 0;

  virtual void errorOnNode(const QString& nodeId, const QString& flowId, const QString& message) = 0;
  virtual void simulateOnNode(const QString& nodeId, const QString& flowId, SimulationProperties options) = 0;

  // Optional later: logger(), settings(), uiHooks(), etc.
};

}  // namespace maki
