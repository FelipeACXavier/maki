#include "simulation_scene.h"

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QJsonDocument>
#include <QJsonValue>
#include <QPainterPath>
#include <QPen>

#include "logging.h"
#include "simulation_label_item.h"

static void addArrow(QGraphicsScene* scene, QPointF from, QPointF to, const QPen& pen, qreal headSize, bool dashed)
{
  QPen p = pen;
  if (dashed)
    p.setStyle(Qt::DashLine);

  scene->addLine(QLineF(from, to), p);

  // Arrow head at "to"
  QLineF line(to, from);
  line.setLength(headSize);

  QLineF left = line;
  left.setAngle(line.angle() + 25);
  QLineF right = line;
  right.setAngle(line.angle() - 25);

  QPolygonF head;
  head << to << left.p2() << right.p2();
  scene->addPolygon(head, p, QBrush(p.color()));
}

TraceSceneBuilder::TraceSceneBuilder(maki::ThemeVars theme, const maki::ThemeFonts& fonts, Style style)
    : mStyle(style)
    , mTheme(theme)
    , mFonts(fonts)
{
  mButtonStyle = new TraceLabelItem::Style{
      QColor(theme.button_pressed_bg),
      QColor(theme.selection_bg),
      QColor(theme.button_bg),
      QPen(QColor(theme.foreground), 1),
      QPen(QColor(theme.foreground), 2),
      fonts.hint};
}

bool TraceSceneBuilder::buildScene(const QJsonObject& traceUpdateMsg, QGraphicsScene* scene, LabelClickHandler clickHandler, QString* errorOut)
{
  scene->clear();

  QVector<RawLifeline> raw;
  if (!parseTraceUpdate(traceUpdateMsg, raw, errorOut))
    return false;

  // NEW: Build ownership model
  ComponentTreeModel model = buildComponentTree(raw, mFonts);

  // Layout maps for leaf lifelines only
  QHash<QString, qreal> xByLeafInstance;

  const auto headerBottom = renderHeader(scene, model, model.root.get(), xByLeafInstance);
  const auto lifelineBottom = renderEvents(scene, model, traceUpdateMsg.value("events").toArray(), traceUpdateMsg.value("lifelines").toArray(), xByLeafInstance, headerBottom);

  // Bottom clickable labels: direct labels + requires replies attached to their leaf
  const qreal lifelineTopY = headerBottom + mStyle.headerToLifelineGap;
  const qreal lifelineBottomY = lifelineBottom + mStyle.headerToLifelineGap;
  renderBottomLabels(scene, model, xByLeafInstance, lifelineTopY, lifelineBottomY, clickHandler);

  const QRectF items = scene->itemsBoundingRect();
  QRectF rect = items.adjusted(-20, -20, 20, 20);
  if (rect.height() < mStyle.sceneMinHeight)
    rect.setHeight(mStyle.sceneMinHeight);

  scene->setSceneRect(rect);
  return true;
}

bool TraceSceneBuilder::parseTraceUpdate(const QJsonObject& traceData, QVector<RawLifeline>& out, QString* errorOut) const
{
  const QJsonArray lifelines = traceData.value("lifelines").toArray();
  if (lifelines.isEmpty())
  {
    if (errorOut)
      *errorOut = "traceData.lifelines is empty.";
    return false;
  }

  out.clear();
  out.reserve(lifelines.size());

  for (const auto& llVal : lifelines)
  {
    const QJsonObject llObj = llVal.toObject();
    const QJsonObject header = llObj.value("header").toObject();

    RawLifeline ll;
    ll.instance = header.value("instance").toString();
    ll.role = header.value("role").toString();
    ll.stateText = extractStateForInstance(traceData, ll.instance);

    const QJsonArray labels = llObj.value("labels").toArray();
    for (const auto& labVal : labels)
    {
      const QJsonObject labObj = labVal.toObject();
      RawLifeline::Label lab;
      lab.text = labObj.value("text").toString();
      lab.role = labObj.value("role").toString();
      lab.illegal = labObj.value("illegal").toBool(false);
      ll.labels.push_back(lab);
    }

    out.push_back(ll);
  }

  return true;
}

QVector<RawLifeline::State> TraceSceneBuilder::extractStateForInstance(const QJsonObject& traceData, const QString& instance) const
{
  const QJsonArray hack = traceData.value("states").toArray();
  if (hack.isEmpty())
    return QVector<RawLifeline::State>();

  const QJsonArray statesOuter = hack.at(0).toArray();
  if (statesOuter.isEmpty())
    return QVector<RawLifeline::State>();

  QVector<RawLifeline::State> states;
  for (const auto& outer : statesOuter)
  {
    const QJsonObject instObj = outer.toObject();
    if (instObj.value("instance").toString() != instance)
      continue;

    const QJsonArray statesInner = instObj.value("state").toArray();
    for (const auto& inner : statesInner)
    {
      const QJsonObject innerObj = inner.toObject();
      states.push_back({innerObj.value("name").toString(), innerObj.value("value").toString()});
    }
  }

  return states;
}

qreal TraceSceneBuilder::renderHeader(QGraphicsScene* scene, const ComponentTreeModel& model, ComponentNode* node, QHash<QString, qreal>& outXByLeafInstance) const
{
  LOG_INFO("Rendering %s", qPrintable(node->fullPath));

  auto* leaf = model.resolveToLeaf(node->fullPath);
  outXByLeafInstance.insert(node->fullPath, node->rect.center().x());

  const QColor fill = headerFillForRole(node->role, leaf != nullptr);

  // Only draw if we are not the root node
  qreal outDiagramTopY = 0;

  // Render this
  if (node->name != "root")
  {
    QPen headerPen(mStyle.headerBorder);
    headerPen.setWidthF(1.2);
    addRoundedRect(scene, node->rect, mStyle.headerRadius, headerPen, QBrush(fill));
    addCenteredText(scene, node->labelRect, node->name, mFonts.label);

    for (int j = 0; leaf && j < leaf->stateText.size(); ++j)
    {
      const auto& state = leaf->stateText.at(j);
      const QString st = QStringLiteral("%1:%2").arg(state.name, state.state);
      addCenteredText(scene, state.rect, st, mFonts.hint);
    }

    outDiagramTopY = node->rect.bottom();
  }

  // Render the children
  for (const auto& child : node->children)
    outDiagramTopY = qMax<qreal>(outDiagramTopY, renderHeader(scene, model, child.get(), outXByLeafInstance));

  return outDiagramTopY;
}

void TraceSceneBuilder::renderBottomLabels(QGraphicsScene* scene, const ComponentTreeModel& model, const QHash<QString, qreal>& xByLeafInstance,
                                           qreal lineTopY, qreal diagramBottomY, LabelClickHandler clickHandler) const
{
  // for (const auto& leafPtr : model.leaves)
  for (int i = 0; i < model.leaves.size(); ++i)
  {
    const auto& leafPtr = model.leaves.at(i);
    const LeafLifeline* leaf = leafPtr.get();
    if (!xByLeafInstance.contains(leaf->instance))
      continue;

    const qreal cx = xByLeafInstance.value(leaf->instance);
    qreal y = diagramBottomY + mStyle.headerToLifelineGap;

    QFontMetrics fm(mFonts.hint);
    auto addClickable = [&](const RawLifeline::Label& label) {
      TraceLabelItem::Payload payload;
      payload.instance = leaf->instance;
      payload.role = "";
      payload.text = label.text;
      payload.call = RawLifeline::Label::lastSegment(label.text);
      payload.illegal = label.illegal;

      // We need to move both the top and the bottom according to y
      auto* item = new TraceLabelItem(label.rect.adjusted(0, y, 0, y), mButtonStyle, payload);
      scene->addItem(item);

      item->clicked = [clickHandler](TraceLabelItem::Payload p) {
        if (clickHandler)
          clickHandler(p.instance, p.text, p.role, p.illegal);
      };

      y += label.rect.height();
    };

    // Lifeline
    QPen linePen(mStyle.lifelineColour);
    linePen.setWidthF(1.0);

    const qreal y1 = lineTopY;
    const qreal y2 = diagramBottomY;
    scene->addLine(QLineF(cx, y1, cx, y2), linePen);

    // Direct labels
    for (const auto& label : leaf->directLabels)
      addClickable(label);
  }
}

qreal TraceSceneBuilder::renderEvents(QGraphicsScene* scene, const ComponentTreeModel& model, const QJsonArray& events,
                                      const QJsonArray& lifelinesJson, const QHash<QString, qreal>& xByLeafInstance, qreal diagramTopY) const
{
  if (events.isEmpty())
    return diagramTopY + 2 * mStyle.headerToLifelineGap;

  // 1) Build activityKey -> (instance, time)
  struct Endpoint
  {
    QString instance;
    int time = 0;
  };
  QHash<int, Endpoint> byKey;

  for (const auto& llVal : lifelinesJson)
  {
    const QJsonObject llObj = llVal.toObject();
    const QString inst = llObj.value("header").toObject().value("instance").toString();
    const QJsonArray acts = llObj.value("activities").toArray();

    for (const auto& aVal : acts)
    {
      const QJsonObject a = aVal.toObject();
      const int key = a.value("key").toInt(-1);
      const int time = a.value("time").toInt(0);
      if (key >= 0)
        byKey.insert(key, Endpoint{inst, time});
    }
  }

  // 2) Draw each event as an arrow between LEAF lifelines
  QPen pen(QColor(mTheme.foreground));
  pen.setWidthF(1.3);

  QHash<int, int> countAtTime;  // spacing when multiple events share the same time

  qreal maxY = diagramTopY;
  QFontMetrics fm(mFonts.hint);
  for (const auto& eVal : events)
  {
    const QJsonObject e = eVal.toObject();
    const int fromKey = e.value("from").toInt(-1);
    const int toKey = e.value("to").toInt(-1);

    if (!byKey.contains(fromKey) || !byKey.contains(toKey))
      continue;

    const Endpoint fromEp = byKey.value(fromKey);
    const Endpoint toEp = byKey.value(toKey);

    // Resolve endpoints to leaf lifelines (THIS is where groups matter)
    LeafLifeline* fromLeaf = model.resolveToLeaf(fromEp.instance);
    LeafLifeline* toLeaf = model.resolveToLeaf(toEp.instance);

    if (!fromLeaf || !toLeaf)
      continue;

    if (!xByLeafInstance.contains(fromLeaf->instance) || !xByLeafInstance.contains(toLeaf->instance))
      continue;

    const qreal x1 = xByLeafInstance.value(fromLeaf->instance);
    const qreal x2 = xByLeafInstance.value(toLeaf->instance);

    const int t = fromEp.time + 1;
    const int k = countAtTime.value(t, 0);
    countAtTime.insert(t, k + 1);

    const qreal y = diagramTopY + t * mStyle.timeStepHeight + k * mStyle.eventVSpacing;
    maxY = qMax<qreal>(maxY, y);

    const QString text = e.value("text").toString();
    const QString type = e.value("type").toString();

    const bool dashed = (type == "out" || type == "return");

    addArrow(scene, QPointF(x1, y), QPointF(x2, y), pen, mStyle.arrowHeadSize, dashed);

    // Label above the arrow
    if (!text.isEmpty())
    {
      auto* label = scene->addSimpleText(text);
      label->setBrush(QColor(mTheme.foreground));
      label->setFont(mFonts.hint);

      const qreal midX = (x1 + x2) / 2.0;
      label->setPos(midX - label->boundingRect().width() / 2.0, y - fm.height());
      label->setZValue(5);
    }
  }

  return maxY;
}

QColor TraceSceneBuilder::headerFillForRole(const QString& role, bool isLeaf) const
{
  if (role == "component")
    return isLeaf ? QColor("#AEE8A0") : QColor("#C9FFC9");
  if (role == "foreign")
    return QColor("#E5FFE5");
  if (role == "requires")
    return QColor("#FFF7A8");
  if (role == "provides")
    return QColor("#FFF7A8");

  // provides or unknown
  return mStyle.headerFillDefault;
}

void TraceSceneBuilder::addRoundedRect(QGraphicsScene* scene, const QRectF& r, qreal radius,
                                       const QPen& pen, const QBrush& brush) const
{
  // QGraphicsRectItem doesn't do rounded corners directly, so draw a path.
  QPainterPath path;
  path.addRoundedRect(r, radius, radius);

  auto* item = scene->addPath(path, pen, brush);
  item->setZValue(0);
}

void TraceSceneBuilder::addCenteredText(QGraphicsScene* scene, const QRectF& within,
                                        const QString& text, const QFont& font, const QColor& colour) const
{
  auto* t = scene->addText(text, font);
  t->setDefaultTextColor(colour);

  const QRectF br = t->boundingRect();
  const qreal x = within.x() + (within.width() - br.width()) / 2.0;
  const qreal y = within.y() + (within.height() - br.height()) / 2.0;
  t->setPos(x, y);
  t->setZValue(2);
}