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
  void generationEnded();
  void openClient(const QString& url);

private:
  Pipeline* mPipeline = nullptr;

  VoidResult generatePipeline(const QString& outputDir, const QStringList& input, QStringList& output);
  VoidResult verifyPipeline(const QStringList& input, QStringList& output);
  VoidResult simulatePipeline(const QStringList& input, QStringList& output);
};
