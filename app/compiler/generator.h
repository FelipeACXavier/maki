#pragma once

#include "result.h"
#include "view/canvas.h"

class Connector;

class Generator
{
public:
  Generator();

  VoidResult generate(Canvas* canvas);

private:
  VoidResult generateFlow(const QGraphicsItem* item, Canvas* canvas);
  VoidResult generateConnector(const std::shared_ptr<Connector> item, Canvas* canvas);
  VoidResult generateNode(const QGraphicsItem* item, Canvas* canvas);
};
