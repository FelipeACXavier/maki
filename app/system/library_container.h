#pragma once

#include <QGraphicsView>
#include <QToolBox>

#include "config.h"
#include "result.h"

class LibraryContainer : public QGraphicsView
{
  Q_OBJECT
public:
  LibraryContainer(QWidget* parent = nullptr);

  static LibraryContainer* create(const QString& name, QToolBox* parent);

  VoidResult addNode(const QString& id, std::shared_ptr<NodeConfig> config);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  int mLastItemY;

  void updateSceneSize();
  void adjustNodePositions();
};
