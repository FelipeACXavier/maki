#pragma once

#include <QGraphicsView>
#include <QJsonValueConstRef>
#include <QToolBox>

#include "result.h"

class DraggableItem;

class LibraryContainer : public QGraphicsView
{
public:
  LibraryContainer(QGraphicsScene* scene);

  static LibraryContainer* create(const QString& name, QToolBox* parent);

  VoidResult addNode(const QJsonValueConstRef& config);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  void adjustNodePositions();
  int getYOfLastItem() const;
};
