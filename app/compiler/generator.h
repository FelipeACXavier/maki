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
};
