#include "koda_arduino_plugin.h"

#include "ast.h"
#include "logging.h"
#include "result.h"

#define IF_ALT(ALT, OBJ, CALL)          \
  if (std::holds_alternative<ALT>(OBJ)) \
  {                                     \
    if (auto obj = std::get<ALT>(OBJ))  \
      return CALL(obj);                 \
  }

#define ELSE_IF_ALT(ALT, OBJ, CALL)          \
  else if (std::holds_alternative<ALT>(OBJ)) \
  {                                          \
    if (auto obj = std::get<ALT>(OBJ))       \
      return CALL(obj);                      \
  }

KodaArduinoPlugin::KodaArduinoPlugin()
{
}

std::string KodaArduinoPlugin::id() const
{
  return "ArduinoPlugin";
}

VoidResult KodaArduinoPlugin::generate(const koda::CompilerOptions& options, const koda::System& ast)
{
  LOG_DEBUG("Running Arduino plugin");
  mOptions = options;

  std::string setupCode = "";
  std::string loopCode = "";

  // For now we do a single file pass
  for (auto& component : ast.components)
  {
    if (component->kind != koda::Component::Kind::Task)
      continue;

    RETURN_ON_FAILURE(generateTask(component));
  }

  std::string filename = std::format("{}/arduino_main.cpp", mOptions.outputDir);
  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return VoidResult::Failed("Failed to open: " + filename);

  file << "std::ostream nullstream(nullptr);\n";
  file << "dzn::runtime runtime;\n";
  file << "dzn::locator locator;\n";
  file << "std::unique_ptr<AlarmSystem> alarmSystem;\n\n";  // TODO: Task name

  file << "void setup()\n";
  file << "{\n";
  file << "  Serial.begin(115200);\n";
  file << "  while(!Serial);\n\n";
  file << "  alarmSystem = std::make_unique<AlarmSystem>(locator\n";
  file << "      .set(runtime)\n";
  file << "      .set(nullstream)\n";
  file << "      .set(updateLoop)\n";
  file << "  );\n";
  file << "}\n\n";

  file << "void loop()\n";
  file << "{\n";
  file << "}\n\n";

  return VoidResult();
}

VoidResult KodaArduinoPlugin::generateTask(koda::PComponent task)
{
  // std::string filename = std::format("{}/a_{}.dzn", mOptions.outputDir, toFilename(capability->name));

  // std::ofstream file;
  // file.open(filename);
  // if (!file.is_open())
  //   return VoidResult::Failed("Failed to open: " + filename);

  for (auto& statement : task->statements)
  {
    IF_ALT(koda::PStrategyBlock, statement->node, generateStrategyBlock)
  }

  return VoidResult();
}

VoidResult KodaArduinoPlugin::generateStrategyBlock(koda::PStrategyBlock strategy)
{
  for (auto& flow : strategy->flows)
  {
    RETURN_ON_FAILURE(generateStrategy(flow->strategy));
  }

  return VoidResult();
}

VoidResult KodaArduinoPlugin::generateStrategy(koda::PStrategy strategy)
{
  IF_ALT(koda::PSeq, strategy->v, generateSequence)
  // ELSE_IF_ALT(koda::PJoin, strategy->v, generateJoin)
  // ELSE_IF_ALT(koda::PEither, strategy->v, generateEither)
  // ELSE_IF_ALT(koda::PLet, strategy->v, generateLet)
  // ELSE_IF_ALT(koda::PWithin, strategy->v, generateWithin)
  // ELSE_IF_ALT(koda::PIfElse, strategy->v, generateIfElse)
  // ELSE_IF_ALT(koda::PRepeat, strategy->v, generateRepeat)
  // ELSE_IF_ALT(koda::PGuard, strategy->v, generateGuard)
  // ELSE_IF_ALT(koda::PEvery, strategy->v, generateEvery)
  // ELSE_IF_ALT(koda::PEnd, strategy->v, generateEnd)
  // ELSE_IF_ALT(koda::PContinue, strategy->v, generateContinue)
  // ELSE_IF_ALT(koda::PRef, strategy->v, generateRef)
  ELSE_IF_ALT(koda::PTaskCall, strategy->v, generateTaskCall)
  // ELSE_IF_ALT(koda::PParen, strategy->v, generateParen)

  return VoidResult();
}

VoidResult KodaArduinoPlugin::generateSequence(koda::PSeq seq)
{
  for (uint32_t i = 0; i < seq->alts.size(); ++i)
  {
    RETURN_ON_FAILURE(generateStrategy(seq->alts[i]));
  }

  return VoidResult();
}

VoidResult KodaArduinoPlugin::generateTaskCall(koda::PTaskCall task)
{
  if (!task->call)
    return VoidResult::Failed("Task with no call");

  LOG_INFO("Generating TaskCall: %s %s", task->call->receiver.c_str(), task->call->name.c_str());

  // const auto receiver = task->call->receiver;
  // const auto event = task->call->name;
  // if (receiver == "led")
  // {
  //   if (event == "start")
  //     mLoopCode += "";
  //   else
  //     mLoopCode += "digitalWrite()";
  // }
  // else if (receiver == "time")
  // {
  // }

  return VoidResult();
}
