#include "component_model.h"

#include <QStringList>

#include "logging.h"

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

ComponentNode* ComponentNode::findOrAddChild(const QString& childName)
{
  for (auto& c : children)
    if (c->name == childName)
      return c.get();

  auto n = std::make_shared<ComponentNode>();
  n->name = childName;
  n->parent = this;

  if (fullPath.isEmpty())
    n->fullPath = childName;
  else
    n->fullPath = fullPath + "." + childName;

  children.push_back(n);
  return n.get();
}

static ComponentNode* ensurePath(ComponentNode* root, const QString& fullInstance)
{
  ComponentNode* cur = root;
  const auto parts = splitPath(fullInstance);
  // LOG_INFO("Looking for children in %s", qPrintable(fullInstance));
  for (const auto& part : parts)
  {
    cur = cur->findOrAddChild(part);
    // if (cur)
    // LOG_INFO("  Found or added child: %s %s", qPrintable(cur->name), qPrintable(cur->fullPath));
  }

  return cur;
}

ComponentTreeModel buildComponentTree(const QVector<RawLifeline>& raw)
{
  ComponentTreeModel model;
  model.root = std::make_shared<ComponentNode>();
  model.root->fullPath = "";
  model.root->name = "root";

  // 1) Ensure all paths exist (including requires entries, so groups are complete)
  for (const auto& ll : raw)
    ensurePath(model.root.get(), ll.instance);

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
    ComponentNode* node = ensurePath(model.root.get(), leaf->instance);
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

  return model;
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