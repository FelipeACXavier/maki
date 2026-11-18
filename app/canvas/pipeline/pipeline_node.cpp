#include "pipeline_node.h"

#include <QPainter>
#include <QSvgRenderer>

#include "config_table.h"
#include "document/property_info.h"
#include "keys.h"
#include "logging.h"
#include "style_helpers.h"

PipelineNode::PipelineNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                           QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
    , mIconRenderer(nullptr)
{
  setAcceptHoverEvents(true);
}

PipelineNode::~PipelineNode()
{
  if (mIconRenderer)
    delete mIconRenderer;
}

VoidResult PipelineNode::start()
{
  // Pipeline nodes only have input and out for now
  mPorts[{Constants::MAIN_SUB_FLOW, Types::Port::IN}] = new PortItem(Types::Port::IN, this);
  mPorts[{Constants::MAIN_SUB_FLOW, Types::Port::OUT}] = new PortItem(Types::Port::OUT, this);

  // Use the capability call icon for the outer and the actual icon for the inner
  if (auto c = ConfigurationTable::instance().get(ConfigKeys::CAPABILITY_CALL_NODE); !c->body.iconPath.isEmpty())
  {
    setRenderer(c->body.iconPath);
    ensureRenderer();
  }

  return BehaviourNode::start();
}

void PipelineNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  NodeItem::paintDefaultNode(painter, style, widget);

  if (mIconRenderer)
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
    paintSvg(mIconRenderer, painter, rect.center(), diameter * 0.8, diameter * 0.8);
  }
}

bool PipelineNode::ensureRenderer()
{
  if (auto configuration = config(); !configuration->body.iconPath.isEmpty())
  {
    LOG_DEBUG("Using icon: {}", iconPathFromTheme(configuration->body.iconPath));
    if (!mIconRenderer)
      mIconRenderer = new QSvgRenderer(iconPathFromTheme(configuration->body.iconPath));
    else
      mIconRenderer->load(iconPathFromTheme(configuration->body.iconPath));

    return true;
  }

  return false;
}
