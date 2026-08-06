#include "component_model.h"

#include <QDebug>
#include <QFontMetricsF>
#include <QStringList>

#include "logging.h"

static const qreal padding = 5;
static const qreal componentMargin = 15;

static QStringList splitPath(const QString& s)
{
  return s.split('.', Qt::SkipEmptyParts);
}

static bool isLeafRole(const QString& role)
{
  // These become actual columns
  // role == "foreign"
  return role == "component" || role == "provides" || role == "requires";
}

QString RawLifeline::Label::lastSegment(const QString& instance)
{
  const int i = instance.lastIndexOf('.');
  return (i >= 0) ? instance.mid(i + 1) : instance;
}

ComponentNode* ComponentNode::findOrAddChild(const QString& childName, const QString& role)
{
  for (auto& c : children)
    if (c->name == childName)
      return c.get();

  auto n = std::make_shared<ComponentNode>();
  n->name = childName;
  n->parent = this;
  n->role = role;

  if (fullPath.isEmpty())
    n->fullPath = childName;
  else
    n->fullPath = fullPath + "." + childName;

  children.push_back(n);
  return n.get();
}

static ComponentNode* ensurePath(ComponentTreeModel* model, ComponentNode* root, const QString& fullInstance, const QString& role)
{
  ComponentNode* cur = root;
  const auto parts = splitPath(fullInstance);
  for (const auto& part : parts)
    cur = cur->findOrAddChild(part, role);

  return cur;
}

QRectF ComponentNode::computeLayout(ComponentTreeModel* model,
                                    const maki::Theme& theme,
                                    int indent)
{
  auto lifeline = model->lifelineOfNode(fullPath);

  QFontMetricsF fmLabel(theme.fontCaption);
  QFontMetricsF fmHint(theme.fontRegular);

  const qreal labelWidth = fmLabel.horizontalAdvance(name);
  const qreal labelHeight = fmLabel.height();

  const qreal lineH = fmHint.height();
  const bool collapsed = !children.empty() && model->isCollapsed(fullPath);

  // Case 1: collapsed group becomes one visible column
  if (collapsed)
  {
    qreal width = qMax<qreal>(labelWidth + 4 * padding, 90);
    qreal height = 2 * padding + labelHeight;

    const qreal cx = model->takeNextColumnX(width);

    rect = QRectF(cx, padding, width, height);
    labelRect = QRectF(cx, 2 * padding, width, labelHeight);

    auto leafLifeline = model->ensureVisualLifeline(fullPath);
    leafLifeline->instance = fullPath;
    leafLifeline->shortName = RawLifeline::Label::lastSegment(fullPath);

    const auto child = children.at(0);
    auto childLifeline = model->lifelineOfNode(child->fullPath);
    if (childLifeline)
    {
      leafLifeline->directLabels += childLifeline->directLabels;

      for (int i = 0; i < leafLifeline->directLabels.size(); ++i)
        leafLifeline->directLabels[i].rect = QRectF(cx, i * lineH, width, lineH);
    }

    return rect;
  }

  // Case 2: actual leaf lifeline
  if (children.empty())
  {
    assert(lifeline != nullptr);
    qreal width = fmLabel.horizontalAdvance(lifeline->shortName);
    qreal height = 2 * padding + labelHeight;

    for (const auto& state : lifeline->stateText)
    {
      const QString st = QStringLiteral("%1:%2").arg(state.name, state.state);
      width = qMax<qreal>(width, fmHint.horizontalAdvance(st) + padding);
      height += lineH;
    }

    for (const auto& event : lifeline->directLabels)
      width = qMax<qreal>(width, fmHint.horizontalAdvance(event.text) + padding);

    const qreal cx = model->takeNextColumnX(width);

    for (int i = 0; i < lifeline->stateText.size(); ++i)
    {
      lifeline->stateText[i].rect =
          QRectF(cx, 2 * padding + labelHeight + i * lineH, width, lineH);
    }

    for (int i = 0; i < lifeline->directLabels.size(); ++i)
      lifeline->directLabels[i].rect = QRectF(cx, i * lineH, width, lineH);

    lifeline->rect = QRectF(cx, padding, width, height);
    lifeline->labelRect = QRectF(cx, 2 * padding, width, labelHeight);

    rect = lifeline->rect;
    labelRect = lifeline->labelRect;

    return rect;
  }

  // Case 3: expanded group wraps visible children
  QRectF bounds;

  for (auto& child : children)
  {
    QRectF childRect = child->computeLayout(model, theme, indent + 2);

    if (!childRect.isNull() && !childRect.isEmpty())
      bounds = bounds.isNull() ? childRect : bounds.united(childRect);
  }

  const qreal headerPadY = padding + labelHeight;
  const qreal headerPadX = padding;

  QRectF groupRect = bounds.adjusted(
      -headerPadX,
      -(headerPadY + padding),
      headerPadX,
      padding);

  groupRect.setWidth(qMax(groupRect.width(), labelWidth + 3 * padding));

  labelRect = QRectF(
      groupRect.left() + padding,
      groupRect.top() + padding,
      groupRect.width(),
      labelHeight);

  rect = groupRect;

  return rect;
}

ComponentTreeModel buildComponentTree(const QVector<RawLifeline>& raw, const maki::Theme& theme, const QSet<QString>& collapsed)
{
  ComponentTreeModel model;
  model.root = std::make_shared<ComponentNode>();
  model.root->fullPath = "";
  model.root->name = ROOT_NODE;
  model.collapsedComponents = collapsed;

  for (const auto& ll : raw)
    ensurePath(&model, model.root.get(), ll.instance, ll.role);

  for (const auto& ll : raw)
  {
    if (!isLeafRole(ll.role))
      continue;

    auto leafLifeline = std::make_shared<LeafLifeline>();
    leafLifeline->instance = ll.instance;
    leafLifeline->shortName = RawLifeline::Label::lastSegment(ll.instance);
    leafLifeline->stateText = ll.stateText;
    leafLifeline->directLabels = ll.labels;

    model.leafByInstance.insert(leafLifeline->instance, leafLifeline);
    model.leaves.push_back(leafLifeline);
  }

  // Uncomment to debug
  // model.print();
  model.computeLayout(theme);

  return model;
}

std::shared_ptr<LeafLifeline> ComponentTreeModel::ensureVisualLifeline(const QString& instance)
{
  auto lifeline = lifelineOfNode(instance);
  if (lifeline)
    return lifeline;

  auto layout = std::make_shared<LeafLifeline>();

  syntheticLeaves.push_back(layout);
  leafByInstance.insert(instance, layout);

  return layout;
}

void ComponentTreeModel::computeLayout(const maki::Theme& theme)
{
  QFontMetricsF fmHint(theme.fontCaption);
  QFontMetricsF fmLabel(theme.fontRegular);
  // qreal lastWidth = 0;

  mLastWidth = 0;

  for (auto& leaf : leaves)
  {
    const qreal lineH = fmHint.height();
    const qreal labelHeight = fmLabel.height();
    const qreal cx = componentMargin + mLastWidth;

    qreal width = fmLabel.horizontalAdvance(leaf->shortName);
    qreal height = (2 * padding) + labelHeight;

    // Get max width of the components
    for (const auto& state : leaf->stateText)
    {
      const QString st = QStringLiteral("%1:%2").arg(state.name, state.state);
      qreal textWidth = fmHint.horizontalAdvance(st) + padding;
      width = qMax<qreal>(width, textWidth);
      height += lineH;
    }

    // Get max width of the events
    for (const auto& event : leaf->directLabels)
    {
      qreal textWidth = fmHint.horizontalAdvance(event.text) + padding;
      width = qMax<qreal>(width, textWidth);
    }

    for (int i = 0; i < leaf->stateText.size(); ++i)
      leaf->stateText[i].rect = QRectF(cx, (2 * padding + labelHeight) + (i * lineH), width, lineH);

    for (int i = 0; i < leaf->directLabels.size(); ++i)
      leaf->directLabels[i].rect = QRectF(cx, i * lineH, width, lineH);

    leaf->rect = QRectF(cx, padding, width, height);
    leaf->labelRect = QRectF(cx, 2 * padding, width, labelHeight);
    // lastWidth += width + componentMargin;
    mLastWidth += width + componentMargin;
  }

  // Compute groups
  root->computeLayout(this, theme, 0);
}

std::shared_ptr<LeafLifeline> ComponentTreeModel::lifelineOfNode(const QString& endpoint) const
{
  if (leafByInstance.contains(endpoint))
    return leafByInstance.value(endpoint);

  return nullptr;
}

std::shared_ptr<LeafLifeline> ComponentTreeModel::resolveToLeaf(const QString& endpoint) const
{
  if (leafByInstance.contains(endpoint))
    return leafByInstance.value(endpoint);

  QString cur = endpoint;
  while (true)
  {
    if (leafByInstance.contains(cur))
      return leafByInstance.value(cur);

    const int dot = cur.lastIndexOf('.');
    if (dot < 0)
      break;

    cur = cur.left(dot);
  }

  return nullptr;
}

bool ComponentTreeModel::isCollapsed(const QString& path) const
{
  return collapsedComponents.contains(path);
}

qreal ComponentTreeModel::takeNextColumnX(qreal width)
{
  const qreal x = componentMargin + mLastWidth;
  mLastWidth += width + componentMargin;
  return x;
}

QString ComponentTreeModel::resolveToVisibleInstance(const QString& endpoint) const
{
  QString cur = endpoint;

  while (!cur.isEmpty())
  {
    if (isCollapsed(cur))
      return cur;

    const int dot = cur.lastIndexOf('.');
    if (dot < 0)
      break;

    cur = cur.left(dot);
  }

  auto leaf = resolveToLeaf(endpoint);
  return leaf ? leaf->instance : endpoint;
}

void ComponentTreeModel::print() const
{
  LOG_INFO("Root:");
  if (root)
    root->print(1);
}

void ComponentNode::print(int level) const
{
  LOG_INFO("{}{} ({}):", std::string(level, ' '), qPrintable(name), qPrintable(fullPath));
  for (const auto& child : children)
    child->print(level + 1);
}
