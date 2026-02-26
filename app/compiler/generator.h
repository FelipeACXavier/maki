#pragma once

#include "result.h"
#include "system/canvas.h"

namespace maki
{
class IGeneratorPlugin;
}

class Pipeline;

class Generator : public QObject
{
  Q_OBJECT
public:
  Generator(Pipeline* pipeline, QObject* parent = nullptr);

  VoidResult generate(const QString& outputDir, maki::IGeneratorPlugin* generator);
  VoidResult simulate(const QString& outputDir, maki::IGeneratorPlugin* generator);

  Pipeline* pipeline() const;
  void setup();

signals:
  void generationStarted(Pipeline* pipeline);
  void generationEnded(const QString& outputFolder);

private:
  Pipeline* mPipeline = nullptr;
  QString mGenerationFolder;
};
