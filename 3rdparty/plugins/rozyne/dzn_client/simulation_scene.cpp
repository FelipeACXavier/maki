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
  ComponentTreeModel model = buildComponentTree(raw);

  // Layout maps for leaf lifelines only
  QHash<QString, qreal> xByLeafInstance;
  qreal diagramTopY = 0;
  qreal diagramBottomY = 0;

  qreal lineTop = mStyle.topMargin + mStyle.headerHeight + mStyle.headerToLifelineGap;
  renderHeadersAndLifelines(scene, model, xByLeafInstance, diagramTopY, diagramBottomY);

  // NEW: draw ownership group boxes above headers
  const qreal headersTopY = mStyle.topMargin;
  renderOwnershipGroups(scene, model, xByLeafInstance, headersTopY);

  auto maxEventY = renderEvents(scene, model, traceUpdateMsg.value("events").toArray(), traceUpdateMsg.value("lifelines").toArray(), xByLeafInstance, diagramTopY);

  // Bottom clickable labels: direct labels + requires replies attached to their leaf
  const qreal labelsTopY = std::max(diagramBottomY, maxEventY + 40);
  renderBottomLabels(scene, model, xByLeafInstance, lineTop, labelsTopY, clickHandler);

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

int TraceSceneBuilder::subtreeMaxDepth(const ComponentNode* n)
{
  int maxD = 0;
  for (const auto& c : n->children)
    maxD = qMax(maxD, 1 + subtreeMaxDepth(c.get()));
  return maxD;
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

void TraceSceneBuilder::renderHeadersAndLifelines(QGraphicsScene* scene, const ComponentTreeModel& model,
                                                  QHash<QString, qreal>& outXByLeafInstance, qreal& outDiagramTopY, qreal& outDiagramBottomY) const
{
  const qreal top = mStyle.topMargin;
  const qreal left = mStyle.leftMargin;

  const qreal headerW = mStyle.headerWidth;
  const qreal headerH = mStyle.headerHeight;

  outDiagramTopY = top + headerH + mStyle.headerToLifelineGap;
  outDiagramBottomY = outDiagramTopY + 220;

  QFontMetricsF fm(mFonts.hint);  // the font you use for state lines
  const qreal lineH = fm.height();

  for (int i = 0; i < model.leaves.size(); ++i)
  {
    const LeafLifeline* leaf = model.leaves[i].get();

    const qreal cx = left + i * mStyle.columnWidth;
    outXByLeafInstance.insert(leaf->instance, cx);

    QRectF headerRect(cx - headerW / 2.0, top, headerW, headerH);
    const QColor fill = headerFillForRole(leaf->role);

    QPen headerPen(mStyle.headerBorder);
    headerPen.setWidthF(1.2);
    addRoundedRect(scene, headerRect, mStyle.headerRadius, headerPen, QBrush(fill));

    // Show short name in header (matches your "s0", "ticket")
    addCenteredText(scene, headerRect.adjusted(8, 4, -8, -headerH / 2.0), leaf->shortName, mFonts.label);

    for (int i = 0; i < leaf->stateText.size(); ++i)
    {
      const auto& state = leaf->stateText.at(i);  // assume {name, state/value}
      const QString st = QStringLiteral("%1:%2").arg(state.name, state.state);

      addCenteredText(scene, headerRect.adjusted(8, (headerH / 2.0 - 2) + i * lineH, -8, -4), st, mFonts.hint);
    }
  }
}

void TraceSceneBuilder::renderOwnershipGroups(QGraphicsScene* scene, const ComponentTreeModel& model,
                                              const QHash<QString, qreal>& xByLeafInstance, qreal headersTopY) const
{
  if (!model.root)
    return;

  // We stack groups above headers, deeper nodes closer to headers.
  // Determine depth by counting segments in fullPath.
  auto depthOf = [](const QString& fullPath) -> int {
    if (fullPath.isEmpty())
      return 0;

    return fullPath.count('.') + 1;
  };

  // Collect nodes (preorder)
  QVector<const ComponentNode*> nodes;
  std::function<void(const ComponentNode*)> walk = [&](const ComponentNode* n) {
    if (!n)
      return;
    if (!n->fullPath.isEmpty())
      nodes.push_back(n);
    for (const auto& c : n->children)
      walk(c.get());
  };
  walk(model.root.get());

  // Sort by depth descending: deepest group drawn lowest (near headers)
  std::sort(nodes.begin(), nodes.end(), [&](const ComponentNode* a, const ComponentNode* b) {
    return depthOf(a->fullPath) > depthOf(b->fullPath);
  });

  QPen pen(QColor(mTheme.foreground));
  pen.setWidthF(1.0);

  for (const ComponentNode* n : nodes)
  {
    // Find x-range
    qreal xMin = std::numeric_limits<qreal>::infinity();
    qreal xMax = -std::numeric_limits<qreal>::infinity();

    for (const LeafLifeline* leaf : n->leavesInSubtree)
    {
      if (!leaf)
        continue;

      if (!xByLeafInstance.contains(leaf->instance))
        continue;

      const qreal x = xByLeafInstance.value(leaf->instance);
      xMin = qMin(xMin, x);
      xMax = qMax(xMax, x);
    }

    if (!std::isfinite(xMin) || !std::isfinite(xMax))
      continue;

    const int depth = depthOf(n->fullPath);

    // y position stacked above headers
    const qreal h = mStyle.groupHeight;
    const qreal y = headersTopY - mStyle.groupTopMargin - depth * (h + mStyle.groupVSpacing);

    const qreal pad = mStyle.headerWidth / 2.0;
    QRectF r(xMin - pad, y, (xMax - xMin) + 2 * pad, h);

    addRoundedRect(scene, r, mStyle.groupRadius, pen, QBrush(mStyle.groupFill));
    addCenteredText(scene, r, n->name, mFonts.label, mStyle.groupText);
  }
}

void TraceSceneBuilder::renderBottomLabels(QGraphicsScene* scene, const ComponentTreeModel& model, const QHash<QString, qreal>& xByLeafInstance,
                                           qreal lineTopY, qreal diagramBottomY, LabelClickHandler clickHandler) const
{
  const qreal baseY = diagramBottomY + 40;
  const qreal h = mStyle.labelBoxHeight;

  // for (const auto& leafPtr : model.leaves)
  for (int i = 0; i < model.leaves.size(); ++i)
  {
    const auto& leafPtr = model.leaves.at(i);
    const LeafLifeline* leaf = leafPtr.get();
    if (!xByLeafInstance.contains(leaf->instance))
      continue;

    const qreal cx = xByLeafInstance.value(leaf->instance);
    qreal y = baseY;

    QFontMetrics fm(mFonts.hint);
    auto addClickable = [&](const QString& text, bool illegal, const QString& operation) {
      qreal textWidth = fm.horizontalAdvance(text);

      // Add horizontal padding
      qreal w = textWidth + mStyle.labelBoxHMargin * 2.0;

      // Clamp to column width
      w = qMin<qreal>(w, mStyle.columnWidth - 16);
      w = qMax<qreal>(w, 60);

      QRectF r(cx - w / 2.0, y, w, h);

      TraceLabelItem::Payload payload;
      payload.instance = leaf->instance;
      payload.role = operation;
      payload.text = text;
      payload.call = RawLifeline::Label::lastSegment(text);
      payload.illegal = illegal;

      auto* item = new TraceLabelItem(r, mButtonStyle, payload);
      scene->addItem(item);

      item->clicked = [clickHandler](TraceLabelItem::Payload p) {
        clickHandler(p.instance, p.text, p.role, p.illegal);
      };

      y += h + mStyle.labelVSpacing;
    };

    // Lifeline
    QPen linePen(mStyle.lifelineColour);
    linePen.setWidthF(1.0);
    const qreal y1 = lineTopY + mStyle.lifelineTopPadding;
    const qreal y2 = diagramBottomY;
    scene->addLine(QLineF(cx, y1, cx, y2), linePen);

    // Direct labels
    for (const auto& lab : leaf->directLabels)
      addClickable(lab.text, lab.illegal, "");
  }
}

qreal TraceSceneBuilder::renderEvents(QGraphicsScene* scene, const ComponentTreeModel& model, const QJsonArray& events,
                                      const QJsonArray& lifelinesJson, const QHash<QString, qreal>& xByLeafInstance, qreal diagramTopY) const
{
  if (events.isEmpty())
    return diagramTopY;

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

    const int t = fromEp.time;  // or min(fromEp.time, toEp.time)
    const int k = countAtTime.value(t, 0);
    countAtTime.insert(t, k + 1);

    const qreal y = diagramTopY + t * mStyle.timeStepHeight + k * mStyle.eventVSpacing;
    maxY = std::max(maxY, y);

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

QColor TraceSceneBuilder::headerFillForRole(const QString& role) const
{
  if (role == "component")
    return mStyle.headerFillComponent;
  if (role == "foreign")
    return mStyle.headerFillForeign;
  if (role == "requires")
    return mStyle.headerFillRequires;
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