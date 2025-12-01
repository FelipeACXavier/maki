#pragma once

#include "result.h"
#include "system/canvas.h"

class GeneratorPlugin;
class Pipeline;

class Generator : public QWidget
{
  Q_OBJECT
public:
  struct GenerationOptions
  {
    Types::GenerationOptions pipeline;
  };

  Generator(std::shared_ptr<SaveInfo> storage, QWidget* parent = nullptr);

  void generate(const QString& outputDir, GeneratorPlugin* generator, const GenerationOptions& option);

signals:
  void generationStarted(Pipeline* pipeline);

private:
  const std::shared_ptr<SaveInfo> mStorage;
  Pipeline* mPipeline;

  VoidResult generatePipeline(const QStringList& files, const QString& outputDir);
  VoidResult verifyPipeline(const QString& outputDir);
  VoidResult simulatePipeline();
};
