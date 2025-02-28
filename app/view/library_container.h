#pragma once

#include <QGraphicsView>
#include <QToolBox>

#include "elements/config.h"
#include "result.h"

class DraggableItem;

class LibraryContainer : public QGraphicsView
{
public:
  LibraryContainer(QGraphicsScene* scene);

  static LibraryContainer* create(const QString& name, QToolBox* parent);

  VoidResult addNode(const QString& id, std::shared_ptr<NodeConfig> config);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  void adjustNodePositions();
  int getYOfLastItem() const;
};
