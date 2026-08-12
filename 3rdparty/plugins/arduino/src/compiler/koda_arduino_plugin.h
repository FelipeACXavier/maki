#pragma once

#include <q23utility.h>

#include "ast/ast.h"
#include "ast/koda_compiler.h"
#include "ast/koda_plugin.h"
#include "result.h"

class KodaArduinoPlugin : public koda::KodaPlugin
{
public:
  KodaArduinoPlugin();

  std::string id() const override;
  VoidResult generate(const koda::CompilerOptions& options, const koda::System& ast) override;

private:
  koda::CompilerOptions mOptions;

  std::string mSetupCode = "";
  std::string mLoopCode = "";

  VoidResult generateTask(koda::PComponent task);
  VoidResult generateSequence(koda::PSeq task);
  VoidResult generateTaskCall(koda::PTaskCall task);

  VoidResult generateStrategy(koda::PStrategy strategy);
  VoidResult generateStrategyBlock(koda::PStrategyBlock strategy);
};