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

  LOG_INFO("Adding child: %s with role: %s", qPrintable(childName), qPrintable(role));

  if (fullPath.isEmpty())
    n->fullPath = childName;
  else
    n->fullPath = fullPath + "." + childName;

  children.push_back(n);
  return n.get();
}

static ComponentNode* ensurePath(ComponentNode* root, const QString& fullInstance, const QString& role)
{
  ComponentNode* cur = root;
  const auto parts = splitPath(fullInstance);
  for (const auto& part : parts)
    cur = cur->findOrAddChild(part, role);

  return cur;
}

QRectF ComponentNode::computeLayout(ComponentTreeModel* model, const maki::ThemeFonts& fonts, int indent)
{
  LOG_INFO("%sCompute layout of %s", std::string(indent, ' ').c_str(), qPrintable(fullPath));

  // If there are children, compute the size of the children
  LeafLifeline* lifeline = nullptr;
  for (const auto& leaf : model->leaves)
  {
    LOG_INFO("%s  Has lifeline %s vs %s?", std::string(indent, ' ').c_str(), qPrintable(leaf->instance), qPrintable(fullPath));
    if (leaf->instance == fullPath)
    {
      lifeline = leaf.get();
      break;
    }
  }

  QFontMetricsF fmLabel(fonts.label);
  const auto labelWidth = fmLabel.horizontalAdvance(name);
  const auto labelHeight = fmLabel.height();

  QRectF bounds;  // null rect (isNull() true)

  auto unite = [&](const QRectF& r) {
    if (r.isNull() || r.isEmpty())
      return;

    bounds = bounds.isNull() ? r : bounds.united(r);
  };

  for (auto& child : children)
  {
    const QRectF childRect = child->computeLayout(model, fonts, indent + 2);
    unite(childRect);
  }

  if (bounds.isNull())
  {
    assert(lifeline != nullptr);
    // NOTE: pick some anchor; here we keep it at (0,0) so caller can place it
    rect = lifeline->rect;
    labelRect = lifeline->labelRect;
  }
  else
  {
    const qreal headerPadY = padding + labelHeight;  // room for title/toggle
    const qreal headerPadX = padding;

    QRectF groupRect = bounds.adjusted(-headerPadX, -(headerPadY + padding), headerPadX, padding);

    // Ensure it's at least wide enough for the title row
    groupRect.setWidth(qMax(groupRect.width(), labelWidth + 3 * padding));

    // 7) Store as this node's rect
    labelRect = QRectF(groupRect.left() + padding, groupRect.top() + padding, groupRect.width(), labelHeight);
    rect = groupRect;
  }

  LOG_INFO("%sDone compute layout of %s (%lf x %lf)", std::string(indent, ' ').c_str(), qPrintable(fullPath), rect.width(), rect.height());
  return rect;
}

ComponentTreeModel buildComponentTree(const QVector<RawLifeline>& raw, const maki::ThemeFonts& fonts)
{
  ComponentTreeModel model;
  model.root = std::make_shared<ComponentNode>();
  model.root->fullPath = "";
  model.root->name = "root";

  // 1) Ensure all paths exist (including requires entries, so groups are complete)
  for (const auto& ll : raw)
    ensurePath(model.root.get(), ll.instance, ll.role);

  // 2) Create leaf lifelines (columns) for leaf roles
  for (const auto& ll : raw)
  {
    if (!isLeafRole(ll.role))
      continue;

    auto leaf = std::make_shared<LeafLifeline>();
    leaf->instance = ll.instance;
    leaf->shortName = RawLifeline::Label::lastSegment(ll.instance);
    leaf->role = ll.role;
    leaf->stateText = ll.stateText;
    leaf->directLabels = ll.labels;

    model.leafByInstance.insert(leaf->instance, leaf.get());

    model.leaves.push_back(leaf);
  }

  // Attach leaves to all ancestors for group-span rendering
  auto attachLeafToAncestors = [&](LeafLifeline* leaf) {
    ComponentNode* node = ensurePath(model.root.get(), leaf->instance, leaf->role);
    for (ComponentNode* cur = node; cur; cur = cur->parent)
    {
      if (leaf->shortName == cur->name)
        continue;

      cur->leavesInSubtree.push_back(leaf);
    }
  };

  for (auto& leafPtr : model.leaves)
    attachLeafToAncestors(leafPtr.get());

  model.print();

  model.computeLayout(fonts);

  return model;
}

void ComponentTreeModel::computeLayout(const maki::ThemeFonts& fonts)
{
  QFontMetricsF fmHint(fonts.hint);
  QFontMetricsF fmLabel(fonts.label);
  qreal lastWidth = 0;

  for (auto& leaf : leaves)
  {
    const qreal lineH = fmHint.height();
    const qreal labelHeight = fmLabel.height();
    const qreal cx = componentMargin + lastWidth;

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
    lastWidth += width + componentMargin;
  }

  // Compute groups
  root->computeLayout(this, fonts, 0);
}

LeafLifeline* ComponentTreeModel::resolveToLeaf(const QString& endpoint) const
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

void ComponentTreeModel::print() const
{
  LOG_INFO("Root:");
  if (root)
    root->print(1);
}

void ComponentNode::print(int level) const
{
  LOG_INFO("%s%s (%s):", std::string(level, ' ').c_str(), qPrintable(name), qPrintable(fullPath));
  for (const auto& child : children)
    child->print(level + 1);
}