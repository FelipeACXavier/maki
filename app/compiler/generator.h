#pragma once

#include "result.h"
#include "system/canvas.h"

class GeneratorPlugin;

class Generator
{
public:
  Generator(std::shared_ptr<SaveInfo> storage);

  void generate(GeneratorPlugin* generator);

private:
  const std::shared_ptr<SaveInfo> mStorage;
};
