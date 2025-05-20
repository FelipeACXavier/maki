#pragma once

#include "result.h"
#include "system/canvas.h"

class GeneratorPlugin;

class Generator
{
public:
  Generator();

  void generate(GeneratorPlugin* generator, Canvas* canvas);
};
