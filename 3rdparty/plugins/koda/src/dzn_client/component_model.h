#pragma once

#include <QHash>
#include <QRectF>
#include <QSet>
#include <QString>
#include <QVector>
#include <memory>

#include "iui.h"

struct ComponentTreeModel;

struct RawLifeline
{
  QString instance;
  QString role;

  struct Label
  {
    QString text;
    QString role;
    bool illegal = false;
    QRectF rect = QRectF();

    static QString lastSegment(const QString& instance);
  };

  struct State
  {
    QString name;
    QString state;
    QRectF rect = QRectF();
  };

  QVector<State> stateText;
  QVector<Label> labels;
};

struct LeafLifeline
{
  QString instance;   // leaf column instance e.g. "sut.ticket" or "sut.reject_flow.s0"
  QString shortName;  // e.g. "ticket", "s0"
  QVector<RawLifeline::State> stateText;

  QVector<RawLifeline::Label> directLabels;

  QRectF rect = QRectF();
  QRectF labelRect = QRectF();
};

struct ComponentNode
{
  QString fullPath;  // e.g. "sut.reject_flow"
  QString name;      // e.g. "reject_flow"
  ComponentNode* parent = nullptr;
  QString role;
  QVector<std::shared_ptr<ComponentNode>> children;

  QRectF rect = QRectF();
  QRectF labelRect = QRectF();

  ComponentNode* findOrAddChild(const QString& childName, const QString& role);
  QRectF computeLayout(ComponentTreeModel* model, const maki::Theme& theme, int indent);
  void print(int level) const;
};

struct ComponentTreeModel
{
  std::shared_ptr<ComponentNode> root;

  QVector<std::shared_ptr<LeafLifeline>> leaves;  // owns
  QVector<std::shared_ptr<LeafLifeline>> syntheticLeaves;
  QHash<QString, std::shared_ptr<LeafLifeline>> leafByInstance;  // "sut.ticket" -> leaf

  QSet<QString> collapsedComponents;
  qreal mLastWidth = 0;

  bool isCollapsed(const QString& path) const;
  qreal takeNextColumnX(qreal width);
  QString resolveToVisibleInstance(const QString& endpoint) const;
  std::shared_ptr<LeafLifeline> ensureVisualLifeline(const QString& instance);

  std::shared_ptr<LeafLifeline> lifelineOfNode(const QString& endpoint) const;
  std::shared_ptr<LeafLifeline> resolveToLeaf(const QString& endpoint) const;
  void computeLayout(const maki::Theme& theme);
  void print() const;
};

static const QString ROOT_NODE = "root";
ComponentTreeModel buildComponentTree(const QVector<RawLifeline>& raw, const maki::Theme& theme, const QSet<QString>& collapsed);
