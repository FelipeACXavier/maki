#include "capability_call_node.h"

#include <QSvgRenderer>

#include "keys.h"
#include "system/config_table.h"

CapabilityCallNode::CapabilityCallNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition,
                                       std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
    , mEmptySlot(nullptr)
    , mCapabilityRenderer(nullptr)
{
  setAcceptHoverEvents(true);
  if (!ensureRenderer())
  {
    mEmptySlot = new EmptySlot(0, this);
    mEmptySlot->clicked = [this](const QPointF& scenePos) {
      if (nodeControlRequested)
        nodeControlRequested(this, scenePos, nullptr);
    };
  }
}

CapabilityCallNode::~CapabilityCallNode()
{
  if (mCapabilityRenderer)
    delete mCapabilityRenderer;
}

VoidResult CapabilityCallNode::start()
{
  if (mEmptySlot)
  {
    mEmptySlot->setDiameter(nodeRect().width() / 3);
    mEmptySlot->setPos(nodeRect().center());
  }

  return VoidResult();
}

void CapabilityCallNode::setProperty(const QString& key, const maki::Value& value)
{
  NodeItem::setProperty(key, value);
  if (key == ConfigKeys::CAPABILITY && ensureRenderer())
  {
    // Update the renderer icon
    if (mEmptySlot)
    {
      delete mEmptySlot;
      mEmptySlot = nullptr;
    }
  }
}

void CapabilityCallNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  NodeItem::paintDefaultNode(painter, style, widget);

  if (mCapabilityRenderer)
  {
    oclero::qlementine::Theme theme;
    if (auto qlementinestyle = oclero::qlementine::appStyle())
      theme = qlementinestyle->theme();

    painter->setPen(QPen(Config::FOREGROUND, 1.0 / baseScale()));
    painter->setBrush(QBrush(theme.neutralColor));

    const auto rect = nodeRect();
    const qreal diameter = rect.width() / 2.5;
    const QRectF r(rect.center().x() - diameter / 2.0, rect.center().y() - diameter / 2.0, diameter, diameter);

    painter->drawEllipse(r);
    paintSvg(mCapabilityRenderer, painter, rect.center(), diameter * 0.8, diameter * 0.8);
  }
}

bool CapabilityCallNode::ensureRenderer()
{
  auto capability = getProperty(ConfigKeys::CAPABILITY);
  if (!capability)
    return false;

  if (auto* value = maki::asValue(capability->getvalue()); value->isRecord())
  {
    auto componentType = maki::recordString(value->toRecord(), ConfigKeys::COMPONENT_TYPE);
    auto config = ConfigurationTable::instance().get(componentType);
    if (config && !config->body.iconPath.isEmpty())
    {
      LOG_DEBUG("Using icon: {} ({}) for {}", config->body.iconPath, iconPathFromTheme(config->body.iconPath), componentType);
      if (!mCapabilityRenderer)
        mCapabilityRenderer = new QSvgRenderer(iconPathFromTheme(config->body.iconPath));
      else
        mCapabilityRenderer->load(iconPathFromTheme(config->body.iconPath));

      return true;
    }
    else
    {
      LOG_DEBUG("No configuration for node: {}", componentType);
    }
  }

  return false;
}
