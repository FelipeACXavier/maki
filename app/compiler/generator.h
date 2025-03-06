#pragma once

#include "result.h"
#include "system/canvas.h"

class Connector;
class GeneratorPlugin;

class Generator
{
public:
  Generator();

  void generate(GeneratorPlugin* generator, Canvas* canvas);

private:
  VoidResult generateFlow(const QGraphicsItem* item, Canvas* canvas);
  VoidResult generateConnector(const std::shared_ptr<Connector> item, Canvas* canvas);
  VoidResult generateNode(const QGraphicsItem* item, Canvas* canvas);
};
