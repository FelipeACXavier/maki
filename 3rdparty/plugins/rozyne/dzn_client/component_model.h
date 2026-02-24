#pragma once

#include <QHash>
#include <QString>
#include <QVector>
#include <memory>

struct RawLifeline
{
  QString instance;
  QString role;

  struct Label
  {
    QString text;
    QString role;
    bool illegal = false;

    static QString lastSegment(const QString& instance);
  };

  struct State
  {
    QString name;
    QString state;
  };

  QVector<State> stateText;
  QVector<Label> labels;
};

struct LeafLifeline
{
  QString instance;   // leaf column instance e.g. "sut.ticket" or "sut.reject_flow.s0"
  QString shortName;  // e.g. "ticket", "s0"
  QString role;       // provides/component/foreign
  QVector<RawLifeline::State> stateText;

  QVector<RawLifeline::Label> directLabels;
};

struct ComponentNode
{
  QString fullPath;  // e.g. "sut.reject_flow"
  QString name;      // e.g. "reject_flow"
  ComponentNode* parent = nullptr;
  QVector<std::shared_ptr<ComponentNode>> children;

  QVector<LeafLifeline*> leavesInSubtree;

  ComponentNode* findOrAddChild(const QString& childName);

  void print(int level) const;
};

struct ComponentTreeModel
{
  std::shared_ptr<ComponentNode> root;

  QVector<std::shared_ptr<LeafLifeline>> leaves;  // owns
  QHash<QString, LeafLifeline*> leafByInstance;   // "sut.ticket" -> leaf

  LeafLifeline* resolveToLeaf(const QString& endpoint) const;

  void print() const;
};

ComponentTreeModel buildComponentTree(const QVector<RawLifeline>& raw);