#pragma once

#include <QWidget>

#include "control_widget.h"

class CanvasView;
class QTreeWidget;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class FlowSaveInfo;

namespace maki
{
class PipelineDataWidget : public maki::ControlWidget
{
  Q_OBJECT

public:
  static PipelineDataWidget* create(const QString& nodeId, std::shared_ptr<FlowSaveInfo> graph, QWidget* parent = nullptr);

  explicit PipelineDataWidget(const QString& nodeId, std::shared_ptr<FlowSaveInfo> graph, QWidget* parent = nullptr);

private:
  enum class Health
  {
    Ok = 0,
    Passable,
    Fail
  };

  void hideMenu();
  using HealthCheckFunction = std::function<Health(const QString& consumableId)>;
  QWidget* createGroup(const QString& title, const QStringList& data, HealthCheckFunction check, QWidget* parent) const;

  Health hasProducer(std::shared_ptr<FlowSaveInfo> graph, const QString& consumableId) const;
  Health hasConsumer(std::shared_ptr<FlowSaveInfo> graph, const QString& productId) const;

  QString healthToColor(Health state) const;
};
}  // namespace maki
