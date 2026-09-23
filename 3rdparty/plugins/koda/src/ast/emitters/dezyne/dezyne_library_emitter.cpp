#include "dezyne_library_emitter.h"

#include <format>

namespace koda::dezyne
{

Result<LibraryComponent> createComponent(Model& model, const std::string& outdir, const std::string& name, SymbolId componentId,
                                         std::function<void(LibraryComponent& component, std::ostringstream& out)> callback)
{
  LibraryComponent component;
  component.name = std::format("c{}", name);
  component.filename = std::format("{}.dzn", name);
  component.path = std::format("{}/lib/{}", outdir, component.filename);

  std::ostringstream out;
  callback(component, out);

  component.symbol = model.declareComponent(component.name, component.path, {componentId}, true, componentId);
  for (const auto& port : component.providesPorts)
    model.declarePort(component.symbol, port.name, PortDirection::Provides, port.kind);

  for (const auto& port : component.requiresPorts)
    model.declarePort(component.symbol, port.name, PortDirection::Requires, port.kind);

  model.setGeneratedFile(component.path, out.str());

  return component;
}

// ===========================================================================================================
// Main interfaces
VoidResult createTypes(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/types.dzn", outdir);
  std::ostringstream out;

  out << "  enum Result {\n";
  out << "    Unknown,\n";
  out << "    Success,\n";
  out << "    Failure,\n";
  out << "    Running,\n";
  out << "    Done,\n";
  out << "    Error\n";
  out << "  };\n\n";

  out << "extern int $int$;\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult createActionInterface(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/iaction.dzn", outdir);

  std::ostringstream out;
  out << "import types.dzn;\n\n";

  out << "interface iaction {\n";
  out << "  in Result trigger();\n";
  out << "  in Result abort();\n";
  out << "  in Result reset();\n\n";

  out << "  out void success();\n";
  out << "  out void failure();\n";
  out << "  out void aborted();\n\n";

  out << "  behavior {\n";
  out << "    enum State { Idle, Running, Error };\n";
  out << "    State state = State.Idle;\n\n";

  out << "    [state.Idle] {\n";
  out << "      on trigger: { state = State.Running; reply(Result.Success); }\n";
  out << "      on trigger: { state = State.Error; reply(Result.Failure); }\n";
  out << "      on trigger: { reply(Result.Done); }\n";
  out << "    }\n\n";

  out << "    [state.Running] {\n";
  out << "      on abort: { state = State.Idle; aborted; reply(Result.Success); }\n";
  out << "      on abort: { state = State.Error; reply(Result.Failure); }\n";
  out << "      on abort: { reply(Result.Running); }\n";
  out << "      on optional: { state = State.Idle; success; }\n";
  out << "      on optional: { state = State.Idle; aborted; }\n";
  out << "      on optional: { state = State.Error; failure; }\n";
  out << "    }\n\n";

  out << "    [state.Error] {\n";
  out << "      on reset: { state = State.Idle; reply(Result.Success); }\n";
  out << "      on reset: { reply(Result.Failure); }\n";
  out << "      on abort: { reply(Result.Error); }\n";
  out << "    }\n";
  out << "  }\n";
  out << "}\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult createSignalInterface(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/isignal.dzn", outdir);

  std::ostringstream out;
  out << "  import types.dzn;\n\n";

  out << "  interface isignal {\n";
  out << "    // in void raise();\n";
  out << "    out void raise();\n\n";

  out << "    behavior {\n";
  out << "      on inevitable: { raise; }\n";
  out << "    }\n";
  out << "  }\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult createExternalInterface(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/iexternal.dzn", outdir);

  std::ostringstream out;
  out << "import types.dzn;\n";
  out << "\n";
  out << "interface iexternal {\n";
  out << "  in Result trigger();\n";
  out << "  in Result abort();\n";
  out << "  in Result reset();\n";
  out << "\n";
  out << "  out void success();\n";
  out << "  out void failure();\n";
  out << "\n";
  out << "  behavior {\n";
  out << "    enum State { Idle, Running, Error };\n";
  out << "    State state = State.Idle;\n";
  out << "\n";
  out << "    [state.Idle] {\n";
  out << "      on trigger: { state = State.Running; reply(Result.Success); }\n";
  out << "      on trigger: { state = State.Error; reply(Result.Failure); }\n";
  out << "      on trigger: { reply(Result.Done); }\n";
  out << "    }\n";
  out << "\n";
  out << "    [state.Running] {\n";
  out << "      on abort: { state = State.Idle; reply(Result.Success); }\n";
  out << "      on abort: { state = State.Error; reply(Result.Failure); }\n";
  out << "      on abort: { reply(Result.Running); }\n";
  out << "\n";
  out << "      on optional: { state = State.Idle; success; }\n";
  out << "      on optional: { state = State.Error; failure; }\n";
  out << "    }\n";
  out << "\n";
  out << "    [state.Error] {\n";
  out << "      on reset: { state = State.Idle; reply(Result.Success); }\n";
  out << "      on reset: { reply(Result.Failure); }\n";
  out << "      on abort: { reply(Result.Error); }\n";
  out << "    }\n";
  out << "  }\n";
  out << "}\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult createAbortInterface(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/iabort.dzn", outdir);

  std::ostringstream out;
  out << "import types.dzn;\n";
  out << "\n";
  out << "interface iabort {\n";
  out << "  in Result abort();\n";
  out << "\n";
  out << "  behavior {\n";
  out << "    on abort: { reply(Result.Success); }\n";
  out << "    on abort: { reply(Result.Running); }\n";
  out << "    on abort: { reply(Result.Failure); }\n";
  out << "  }\n";
  out << "}\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult createConditionInterface(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/icondition.dzn", outdir);

  std::ostringstream out;
  out << "enum ConditionResult { True, False };\n";
  out << "\n";
  out << "interface icondition {\n";
  out << "  in ConditionResult evaluate();\n";
  out << "\n";
  out << "  behaviour {\n";
  out << "    on evaluate: reply(ConditionResult.True);\n";
  out << "    on evaluate: reply(ConditionResult.False);\n";
  out << "  }\n";
  out << "}\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

// ===========================================================================================================
// Main orchestration components
Result<LibraryComponent> createSequenceComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  auto numberedName = std::format("sequence{}", instances);
  return createComponent(model, outdir, numberedName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    for (uint32_t i = 0; i < instances; ++i)
      component.requiresPorts.push_back({std::format("action{}", i), PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n\n";

    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n\n";

    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("  requires iaction action{};\n", i);

    out << "\n";

    out << "  behaviour {\n";
    out << "    enum State { Idle, ";
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("Action{}, Aborting{}, ", i, i);

    out << "Error };\n";
    out << "    State state = State.Idle;\n\n";

    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        Result ret = action0.trigger();\n";
    out << "        if (ret.Success) {\n";
    out << "          state = State.Action0;\n";
    out << "        } else if (ret.Done) {\n";
    createSequenceDoneRecursion(true, 1, instances, out, "          ");
    out << "        } else {\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "        reply(ret);\n";
    out << "      }\n";
    out << "    }\n\n";

    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("    [state.Action{}] {{\n", i);
      out << std::format("      on action{}.success(): {{\n", i);
      if (i + 1 == instances)
      {
        out << "        api.success();\n";
        out << "        state = State.Idle;\n";
      }
      else
      {
        out << std::format("        Result ret = action{}.trigger();\n", i + 1);
        out << "        if (ret.Success) {\n";
        out << std::format("          state = State.Action{};\n", i + 1);
        out << "        } else if (ret.Done) {\n";
        createSequenceDoneRecursion(false, i + 2, instances, out, "          ");
        out << "        } else {\n";
        out << "          api.failure();\n";
        out << "          state = State.Error;\n";
        out << "        }\n";
      }
      out << "      }\n\n";

      out << std::format("      on action{}.failure(): {{\n", i);
      out << "        api.failure();\n";
      out << "        state = State.Error;\n";
      out << "      }\n\n";

      out << std::format("      on action{}.aborted(): {{\n", i);
      out << "        api.aborted();\n";
      out << "        state = State.Idle;\n";
      out << "      }\n\n";

      out << "      on api.abort(): {\n";
      out << std::format("        Result ret = action{}.abort();\n", i);
      out << "        if (ret.Success)\n";
      out << std::format("          state = State.Aborting{};\n", i);
      out << "        else if (ret.Failure)\n";
      out << "          state = State.Error;\n";
      out << "        reply(ret);\n";
      out << "      }\n";
      out << "    }\n\n";

      out << std::format("    [state.Aborting{}] {{\n", i);
      out << std::format("      on action{}.aborted(): {{\n", i);
      out << "        api.aborted();\n";
      out << "        state = State.Idle;\n";
      out << "      }\n";
      out << "    }\n\n";
    }

    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    for (uint32_t i = 0; i < instances; ++i)
    {
      if (i == 0)
        out << std::format("        if (action{}.state.Error) {{\n", i);
      else
        out << std::format("        else if (action{}.state.Error) {{\n", i);
      out << std::format("          Result ret = action{}.reset();\n", i);
      out << "          if (ret.Success)\n";
      out << "            state = State.Idle;\n";
      out << "          reply(ret);\n";
      out << "        }\n";
    }
    out << "      }\n\n";

    out << "      on api.abort(): {\n";
    for (uint32_t i = 0; i < instances; ++i)
    {
      if (i == 0)
        out << std::format("        if (action{}.state.Error)\n", i);
      else
        out << std::format("        else if (action{}.state.Error)\n", i);

      out << std::format("          reply(action{}.abort());\n", i);
    }
    out << "      }\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createParallelComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  auto numberedName = std::format("parallel{}", instances);
  return createComponent(model, outdir, numberedName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});

    for (uint32_t i = 0; i < instances; ++i)
      component.requiresPorts.push_back({std::format("action{}", i), PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";

    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n";

    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("  requires iaction action{};\n", i);

    out << "  behaviour {\n";
    out << "    enum State { Idle, Running, Aborting, Error };\n";
    out << std::format("    subint Completed {{0..{}}};\n", instances);

    out << "    State state = State.Idle;\n";
    out << "    Completed completed = 0;\n";

    // ------------------------------------------------------------------
    // Idle state
    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        completed = 0;\n";
    out << "        Result ret = action0.trigger();\n";
    out << "        if (ret.Success) {\n";
    createParallelDoneRecursion(true, false, 1, instances, out, "          ");
    out << "        } else if (ret.Done) {\n";
    out << "          completed = completed + 1;\n";
    createParallelDoneRecursion(true, true, 1, instances, out, "          ");
    out << "        } else {\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "        reply(ret);\n";
    out << "      }\n";
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("      on action{}.aborted(): {{}}\n", i);

    out << "    }\n\n";

    // ------------------------------------------------------------------
    // Running state
    out << "    [state.Running] {\n";
    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("      on action{}.success(): {{\n", i);
      out << "        completed = completed + 1;\n";
      out << std::format("        if (completed == {}) {{\n", instances);
      out << "          api.success();\n";
      out << "          state = State.Idle;\n";
      out << "        }\n";
      out << "      }\n";

      out << std::format("      on action{}.aborted(): {{\n", i);
      out << "        completed = completed + 1;\n";
      out << std::format("        if (completed == {}) {{\n", instances);
      out << "          api.aborted();\n";
      out << "          state = State.Idle;\n";
      out << "        }\n";
      out << "      }\n";

      out << std::format("      on action{}.failure(): {{\n", i);
      for (uint32_t j = 0; j < instances; ++j)
      {
        if (i == j)
          continue;

        out << std::format("        if (action{}.state.Running)\n", j);
        out << std::format("          Result abrt{} = action{}.abort();\n", j, j);
      }

      out << "        api.failure();\n";
      out << "        state = State.Error;\n";
      out << "      }\n";
    }

    out << "      on api.abort(): {\n";

    std::string conjunction = "";
    std::string disjunction = "";
    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("        Result ret{} = Result.Success;\n", i);
      out << std::format("        if (action{}.state.Running)\n", i);
      out << std::format("          ret{} = action{}.abort();\n\n", i, i);

      conjunction += (i != 0 ? " && " : "") + std::format("ret{}.Success", i);
      disjunction += (i != 0 ? " || " : "") + std::format("ret{}.Failure", i);
    }

    out << std::format("        if ({}) {{\n", conjunction);
    out << "          state = State.Aborting;\n";
    out << "          reply(Result.Success);\n";
    out << std::format("        }} else if ({}) {{\n", disjunction);
    out << "          state = State.Error;\n";
    out << "          reply(Result.Failure);\n";
    out << "        } else {\n";
    out << "          reply(Result.Running);\n";
    out << "        }\n";
    out << "      }\n";
    out << "    }\n";  // Running state

    // ------------------------------------------------------------------
    // Aborting state
    out << "    [state.Aborting] {\n";
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("      on action{}.aborted(): {{\n"
                         "        completed = completed + 1;\n"
                         "        if (completed == {}) {{\n"
                         "          api.aborted();\n"
                         "          state = State.Idle;\n"
                         "        }}\n"
                         "      }}\n",
                         i, instances);
    out << "}\n";

    // ------------------------------------------------------------------
    // Error state
    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";

    std::string conjunctionError = "";
    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("        Result ret{} = Result.Success;\n", i);
      out << std::format("        if (action{}.state.Error)\n", i);
      out << std::format("          ret{} = action{}.reset();\n", i, i);
      out << std::format("        else if (action{}.state.Running)\n", i);
      out << std::format("          ret{} = action{}.abort();\n\n", i, i);

      conjunctionError += (i != 0 ? " && " : "") + std::format("ret{}.Success", i);
    }

    out << std::format("        if ({}) {{\n", conjunctionError);
    out << "          state = State.Idle;\n";
    out << "          reply(Result.Success);\n";
    out << "        } else {\n";
    out << "          reply(Result.Failure);\n";
    out << "        }\n";
    out << "      }\n\n";

    out << "      on api.abort(): { reply(Result.Error); }\n\n";

    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("      on action{}.success(): {{}}\n", i);
      out << std::format("      on action{}.failure(): {{}}\n", i);
      out << std::format("      on action{}.aborted(): {{}}\n", i);
    }
    out << "    }\n";  // Error state
    out << "  }\n";    // Behaviour
    out << "}\n";      // Component
  });
}

Result<LibraryComponent> createEveryComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "every", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    component.providesPorts.push_back({"alarm", PortProtocol::Alarm});
    component.requiresPorts.push_back({"action", PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import ialarm.dzn;\n";
    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n";
    out << "  requires ialarm alarm;\n";
    out << "  requires iaction action;\n";
    out << "  behaviour {\n";
    out << "    enum State { Idle, Running, State2, Error };\n";
    out << "    State state = State.Idle;\n";
    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        alarm.set($30$);\n";
    out << "        state = State.Running;\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n";
    out << "      // on api.abort(): { reply(Result.Success); }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.Running] {\n";
    out << "      on alarm.timeout(): {\n";
    out << "        Result triggered = action1.trigger();\n";
    out << "        if (triggered.Error) {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        } else {\n";
    out << "          state = State.State2;\n";
    out << "        }\n";
    out << "      }\n";
    out << "\n";
    out << "      on api.abort(): {\n";
    out << "        alarm.reset();\n";
    out << "        state = State.Idle;\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.State2] {\n";
    out << "      on action1.success(): {\n";
    out << "        alarm.set($30$);\n";
    out << "        state = State.Running;\n";
    out << "      }\n";
    out << "\n";
    out << "      on action1.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "\n";
    out << "      on api.abort(): {\n";
    out << "        alarm.reset();\n";
    out << "\n";
    out << "        Result res = action1.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.Idle;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        Result reset = action1.reset();\n";
    out << "        if (reset.Success)\n";
    out << "          state = State.Idle;\n";
    out << "\n";
    out << "        reply(reset);\n";
    out << "      }\n";
    out << "\n";
    out << "      on api.abort(): { reply(Result.Error); }\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createWithinComponent(Model& model, const std::string& outdir, int timeout, SymbolId componentId)
{
  const auto componentName = std::format("within{}", timeout);
  return createComponent(model, outdir, componentName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    component.providesPorts.push_back({"alarm", PortProtocol::Alarm});
    component.requiresPorts.push_back({"actionDo", PortProtocol::Action});
    component.requiresPorts.push_back({"actionElse", PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import ialarm.dzn;\n\n";

    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n";
    out << "  requires ialarm alarm;\n";
    out << "  requires iaction actionDo;\n";
    out << "  requires iaction actionElse;\n\n";

    out << "  behaviour {\n";
    out << "    enum State { Idle, Do, AbortingDo, Waiting, Else, AbortingElse, Error };\n";
    out << "    State state = State.Idle;\n\n";

    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        Result triggered = actionDo.trigger();\n";
    out << "        if (triggered.Success) {\n";
    out << std::format("          alarm.set(${}$);\n", timeout);
    out << "          state = State.Do;\n";
    out << "        } else if (triggered.Failure) {\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "        reply(triggered);\n";
    out << "      }\n";
    out << "\n";
    out << "      on actionDo.aborted(): {}\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.Do] {\n";
    out << "      on alarm.timeout(): {\n";
    out << "        Result aborted = actionDo.abort();\n";
    out << "        if (aborted.Success) {\n";
    out << "          Result handled = actionElse.trigger();\n";
    out << "          if (handled.Success) {\n";
    out << "            state = State.Else;\n";
    out << "          } else if (handled.Failure) {\n";
    out << "            api.failure();\n";
    out << "            state = State.Error;\n";
    out << "          } else {\n";
    out << "            api.success();\n";
    out << "            state = State.Idle;\n";
    out << "          }\n";
    out << "        } else if (aborted.Running) {\n";
    out << "          state = State.Waiting;\n";
    out << "        } else {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on actionDo.success(): {\n";
    out << "        alarm.reset();\n";
    out << "        api.success();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on actionDo.failure(): {\n";
    out << "        alarm.reset();\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on actionDo.aborted(): {\n";
    out << "        alarm.reset();\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        alarm.reset();\n";
    out << "        Result res = actionDo.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.AbortingDo;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.AbortingDo] {\n";
    out << "      on actionDo.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.Waiting] {\n";
    out << "      on actionDo.success(): {\n";
    out << "        Result handled = actionElse.trigger();\n";
    out << "        if (handled.Success) {\n";
    out << "          state = State.Else;\n";
    out << "        } else if (handled.Failure) {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        } else {\n";
    out << "          api.success();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on actionDo.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on actionDo.aborted(): {\n";
    out << "        Result handled = actionElse.trigger();\n";
    out << "        if (handled.Success) {\n";
    out << "          state = State.Else;\n";
    out << "        } else if (handled.Failure) {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        } else {\n";
    out << "          api.success();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result res = actionDo.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.AbortingDo;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.Else] {\n";
    out << "      on actionElse.success(): {\n";
    out << "        api.success();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on actionElse.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on actionDo.aborted(): {}\n";
    out << "      on actionElse.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result res = actionElse.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.AbortingElse;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.AbortingElse] {\n";
    out << "      on actionElse.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        Result ret1 = Result.Success;\n";
    out << "        Result ret2 = Result.Success;\n";
    out << "        if (actionDo.state.Error) {\n";
    out << "          ret1 = actionDo.reset();\n";
    out << "        }\n";
    out << "        if (actionElse.state.Error) {\n";
    out << "          ret2 = actionElse.reset();\n";
    out << "        }\n";
    out << "        if (ret1.Success && ret2.Success) {\n";
    out << "          state = State.Idle;\n";
    out << "          reply(Result.Success);\n";
    out << "        } else {\n";
    out << "          reply(Result.Failure);\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on api.abort(): { reply(Result.Error); }\n";
    out << "      on actionDo.success(): {}\n";
    out << "      on actionDo.failure(): {}\n";
    out << "      on actionDo.aborted(): {}\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createRepeatComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "repeat", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    component.requiresPorts.push_back({"action", PortProtocol::Action});

    out << "  import types.dzn;\n";
    out << "  import iaction.dzn;\n";
    out << "  component crepeat {\n";
    out << "    provides iaction api;\n";
    out << "    requires iaction action;\n\n";

    out << "    behaviour {\n";
    out << "      enum State { Idle, Running, Aborting, Error };\n";
    out << "      State state = State.Idle;\n";
    out << "  \n";
    out << "      [state.Idle] {\n";
    out << "        on api.trigger(): {\n";
    out << "          Result ret = action.trigger();\n";
    out << "          if (ret.Success)\n";
    out << "            state = State.Running;\n";
    out << "          else if (ret.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(ret);\n";
    out << "        }  \n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.Running] {\n";
    out << "        on action.success(): {\n";
    out << "          Result res1 = action.trigger();\n";
    out << "          if (res1.Failure) {\n";
    out << "            api.failure();\n";
    out << "            state = State.Error;\n";
    out << "          } else if (res1.Done) { // This should never happen\n";
    out << "            api.success();\n";
    out << "            state = State.Idle;\n";
    out << "          }\n";
    out << "        }\n\n";

    out << "        on action.failure(): {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n\n";

    out << "        on action.aborted(): {\n";
    out << "          api.aborted();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n\n";

    out << "        on api.abort(): {\n";
    out << "          Result res = action.abort();\n";
    out << "          if (res.Success)\n";
    out << "            state = State.Aborting;\n";
    out << "          else if (res.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res);\n";
    out << "        }\n";
    out << "      }\n\n";

    out << "      [state.Aborting] {\n";
    out << "        on action.aborted(): {\n";
    out << "          api.aborted();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "      }\n\n";

    out << "      [state.Error] {\n";
    out << "        on api.reset(): {\n";
    out << "          Result reset = action.reset();\n";
    out << "          if (reset.Success)\n";
    out << "            state = State.Idle;\n";
    out << "  \n";
    out << "          reply(reset);\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): { reply(Result.Error); }\n";
    out << "      }\n";
    out << "    }\n";
    out << "  }\n";
  });
}

Result<LibraryComponent> createSignalHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return LibraryComponent{};
}

Result<LibraryComponent> createSignalContinueComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return LibraryComponent{};
}

Result<LibraryComponent> createAbortHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "abort_handler", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    component.requiresPorts.push_back({"action", PortProtocol::Action});
    component.requiresPorts.push_back({"handler", PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import isignal.dzn;\n";
    out << "\n";
    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n";
    out << "\n";
    out << "  requires iaction action;\n";
    out << "  requires iaction handler;\n";
    out << "\n";
    out << "  behaviour {\n";
    out << "    enum State { Idle, Running, Handling, Error };\n";
    out << "    State state = State.Idle;\n";
    out << "  \n";
    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        Result started = action.trigger();\n";
    out << "        if (started.Success)\n";
    out << "          state = State.Running;\n";
    out << "        else if (started.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(started);\n";
    out << "      }\n\n";

    out << "      on action.aborted(): {}\n";
    out << "      on handler.aborted(): {}\n";
    out << "    }\n\n";

    out << "    [state.Running] {\n";
    out << "      on action.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n\n";

    out << "      on action.success(): {\n";
    out << "        api.success();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n\n";

    out << "      on action.aborted(): {\n";
    out << "        Result handled = handler.trigger();\n";
    out << "        if (handled.Success) {\n";
    out << "          state = State.Handling;\n";
    out << "        } else if (handled.Failure) {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        } else {\n";
    out << "          api.success();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "      }\n\n";

    out << "      on api.abort(): {\n";
    out << "        Result aborted = action.abort();\n";
    out << "        if (aborted.Success) {\n";
    out << "          Result handled = handler.trigger();\n";
    out << "          if (handled.Success) {\n";
    out << "            state = State.Handling;\n";
    out << "            reply(Result.Running);\n";
    out << "          } else if (handled.Failure) {\n";
    out << "            state = State.Error;\n";
    out << "            reply(handled);\n";
    out << "          } else {\n";
    out << "            api.aborted();\n";
    out << "            state = State.Idle;\n";
    out << "            reply(Result.Success);\n";
    out << "          }\n";
    out << "        } else if (aborted.Failure) {\n";
    out << "          state = State.Error;\n";
    out << "          reply(aborted);\n";
    out << "        } else {\n";
    out << "          reply (aborted);\n";
    out << "        }\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Handling] {\n";
    out << "      on handler.success(): {\n";
    out << "        api.success();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on handler.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on handler.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result res = handler.abort();\n";
    out << "        if (res.Success)\n";
    out << "        {\n";
    out << "          api.aborted();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "      on action.success(): {}\n";
    out << "      on action.failure(): {}\n";
    out << "      on action.aborted(): {}\n";
    out << "    }\n\n";

    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        Result ret1 = Result.Success;\n";
    out << "        Result ret2 = Result.Success;\n";
    out << "        if (action.state.Error) {\n";
    out << "          ret1 = action.reset();\n";
    out << "        }\n";
    out << "        if (handler.state.Error) {\n";
    out << "          ret2 = handler.reset();\n";
    out << "        }\n";
    out << "        if (ret1.Success && ret2.Success) {\n";
    out << "          state = State.Idle;\n";
    out << "          reply(Result.Success);\n";
    out << "        } else {\n";
    out << "          reply(Result.Failure);\n";
    out << "        }\n";
    out << "      }\n\n";

    out << "      on api.abort(): { reply(Result.Error); }\n\n";

    out << "      on action.success(): {}\n";
    out << "      on action.failure(): {}\n";
    out << "      on action.aborted(): {}\n";
    out << "      on handler.success(): {}\n";
    out << "      on handler.failure(): {}\n";
    out << "      on handler.aborted(): {}\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createErrorHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "error_handler", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    component.requiresPorts.push_back({"action", PortProtocol::Action});
    component.requiresPorts.push_back({"handler", PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n\n";

    out << std::format("  component {} {{\n", component.name);
    out << "  provides iaction api;\n\n";
    out << "  requires iaction action;\n";
    out << "  requires iaction handler;\n";
    out << "  behaviour {\n";
    out << "    enum State { Idle, Running, Handling, AbortingAction, AbortingHandler, Error };\n";
    out << "    State state = State.Idle;\n\n";

    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        Result res1 = action.trigger();\n";
    out << "        if (res1.Success)\n";
    out << "          state = State.Running;\n";
    out << "        else if (res1.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res1);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Running] {\n";
    out << "      on action.failure(): {\n";
    out << "        // Does this make sense? Shouldn't the error handler keep the error and still do something?\n";
    out << "        Result reset = action.reset();\n";
    out << "        if (reset.Success) {\n";
    out << "          Result res = handler.trigger();\n";
    out << "          if (res.Success) {\n";
    out << "            state = State.Handling;\n";
    out << "          } else if (res.Done) {\n";
    out << "            api.success();\n";
    out << "            state = State.Idle;\n";
    out << "          } else {\n";
    out << "            api.failure();\n";
    out << "            state = State.Error;\n";
    out << "          }\n";
    out << "        } else {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on action.success(): {\n";
    out << "        api.success();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on action.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result res = action.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.AbortingAction;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.AbortingAction] {\n";
    out << "      on action.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Handling] {\n";
    out << "      on handler.success(): {\n";
    out << "        api.success();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on handler.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on handler.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result res = handler.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.AbortingHandler;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.AbortingHandler] {\n";
    out << "      on handler.aborted(): {\n";
    out << "        api.aborted();\n";
    out << "        state = State.Idle;\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        Result ret1 = Result.Success;\n";
    out << "        Result ret2 = Result.Success;\n";
    out << "        if (action.state.Error) {\n";
    out << "          ret1 = action.reset();\n";
    out << "        }\n";
    out << "        if (handler.state.Error) {\n";
    out << "          ret2 = handler.reset();\n";
    out << "        }\n";
    out << "        if (ret1.Success && ret2.Success) {\n";
    out << "          state = State.Idle;\n";
    out << "          reply(Result.Success);\n";
    out << "        } else {\n";
    out << "          reply(Result.Failure);\n";
    out << "        }\n";
    out << "      }\n\n";

    out << "      on api.abort(): { reply(Result.Error); }\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createFailureComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "failure", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n\n";

    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n\n";

    out << "  behaviour {\n";
    out << "    enum State { Idle, Error };\n";
    out << "    State state = State.Idle;\n\n";

    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        state = State.Error;\n";
    out << "        reply(Result.Failure);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        state = State.Idle;\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        reply(Result.Error);\n";
    out << "      }\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createSelectorComponent(Model& model, const std::string& outdir, uint32_t conditionCount, uint32_t branchCount, SymbolId componentId)
{
  const auto componentName = std::format("selector{}", branchCount);
  return createComponent(model, outdir, componentName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});

    for (size_t i = 0; i < conditionCount; ++i)
      component.requiresPorts.push_back({std::format("condition{}", i), PortProtocol::Condition});

    for (size_t i = 0; i < branchCount; ++i)
      component.requiresPorts.push_back({std::format("flow{}", i), PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import icondition.dzn;\n\n";

    out << std::format("component {} {{\n", component.name);

    out << "  provides iaction api;\n\n";

    for (size_t i = 0; i < conditionCount; ++i)
      out << std::format("  requires icondition condition{};\n", i);

    if (conditionCount > 0)
      out << "\n";

    for (size_t i = 0; i < branchCount; ++i)
      out << std::format("  requires iaction flow{};\n", i);

    out << "\n";
    out << "  behaviour {\n";

    out << "    enum State { Idle";

    for (size_t i = 0; i < branchCount; ++i)
      out << std::format(", Flow{}", i);

    out << ", Error };\n";
    out << "    State state = State.Idle;\n\n";

    // ----------------------------------------------------------
    // Idle selection
    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    createSelectorRecursion(0, conditionCount, out, "        ");
    out << "      }\n";
    out << "    }\n\n";

    // ----------------------------------------------------------
    // Active branch states
    for (size_t i = 0; i < branchCount; ++i)
    {
      out << std::format("    [state.Flow{}] {{\n", i);

      out << std::format("      on flow{}.success(): {{\n"
                         "        api.success();\n"
                         "        state = State.Idle;\n"
                         "      }}\n\n",
                         i);

      out << std::format("      on flow{}.failure(): {{\n"
                         "        api.failure();\n"
                         "        state = State.Error;\n"
                         "      }}\n\n",
                         i);

      out << std::format("      on api.abort(): {{\n"
                         "        Result ret = flow{}.abort();\n"
                         "        if (ret.Success)\n"
                         "          state = State.Idle;\n"
                         "        else if (ret.Failure)\n"
                         "          state = State.Error;\n"
                         "\n"
                         "        reply(ret);\n"
                         "      }}\n",
                         i);

      out << "    }\n\n";
    }

    // ----------------------------------------------------------
    // Error state
    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        Result ret = Result.Success;\n";

    for (size_t i = 0; i < branchCount; ++i)
    {
      out << std::format("        if (flow{}.state.Error) {{\n"
                         "          Result reset = flow{}.reset();\n"
                         "          if (reset.Failure)\n"
                         "            ret = reset;\n"
                         "        }}\n",
                         i, i);
    }

    out << "        if (ret.Success)\n";
    out << "          state = State.Idle;\n";
    out << "        reply(ret);\n";
    out << "      }\n\n";

    out << "      on api.abort(): { reply(Result.Error); }\n";
    out << "    }\n";

    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createAbortCallComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "abort_call", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Action});
    component.requiresPorts.push_back({"action", PortProtocol::Abort});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import iabort.dzn;\n";
    out << "\n";
    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n";
    out << "  requires iabort action;\n";
    out << "\n";
    out << "  behaviour {\n";
    out << "    enum State { Idle, Aborting, Error };\n";
    out << "    State state = State.Idle;\n";
    out << "\n";
    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        Result res1 = action.abort();\n";
    out << "        if (res1.Failure)\n";
    out << "        {\n";
    out << "          state = State.Error;\n";
    out << "          reply(Result.Failure);\n";
    out << "        }\n";
    out << "        else\n";
    out << "        {\n";
    out << "          state = State.Aborting;\n";
    out << "          defer(state) { state = State.Idle; api.success(); }\n";
    out << "          reply(Result.Success);\n";
    out << "        }\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Aborting] {\n";
    out << "      on api.abort(): {\n";
    out << "        state = State.Idle;\n";
    out << "        api.aborted();\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        state = State.Idle;\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n\n";

    out << "      on api.abort(): { reply(Result.Error); }\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createAbortArbiterComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  const auto componentName = std::format("abort_arbiter{}", instances);
  return createComponent(model, outdir, componentName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    for (uint32_t i = 0; i < instances; ++i)
      component.providesPorts.push_back({std::format("client{}", i), PortProtocol::Abort});

    component.requiresPorts.push_back({"resource", PortProtocol::Abort});

    out << "import types.dzn;\n";
    out << "import iabort.dzn;\n";
    out << "\n";
    out << std::format("component {} {{\n", component.name);
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("  provides iabort client{};\n", i);

    out << "\n";
    out << "  requires iabort resource;\n";
    out << "\n";
    out << "  behaviour {\n";
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("    on client{}.abort(): {{ reply(resource.abort()); }}\n", i);
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createActionArbiterComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  const auto componentName = std::format("action_arbiter{}", instances);
  return createComponent(model, outdir, componentName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    for (uint32_t i = 0; i < instances; ++i)
      component.providesPorts.push_back({std::format("client{}", i), PortProtocol::Action});
    component.requiresPorts.push_back({"resource", PortProtocol::Action});

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n\n";

    out << std::format("component {} {{\n", component.name);
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("  provides iaction client{};\n", i);

    out << "\n  requires iaction resource;\n\n";

    out << "  behaviour {\n";
    out << "    enum Owner { None, ";
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("C{}{}", i, (i + 1 == instances ? "" : ", "));
    out << "};\n";
    out << "    Owner owner = Owner.None;\n";
    out << "    Owner pending = Owner.None;\n";
    out << "    bool erroring = false;\n";
    out << "    bool succeeding = false;\n\n";

    out << "    Result handleAbort()\n";
    out << "    {\n";
    out << "      Result ret = resource.abort();\n";
    out << "      if (ret.Success)\n";
    out << "      {\n";
    out << "        owner = Owner.None;\n";
    out << "        pending = Owner.None;\n";
    out << "      }\n\n";

    out << "      return ret;\n";
    out << "    }\n\n";

    out << "    [owner.None] {\n";
    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("      on client{}.trigger(): {{\n", i);
      out << "        if (erroring) {\n";
      out << "          reply(Result.Failure);\n";
      out << "        } else {\n";
      out << "          Result ret = resource.trigger();\n";
      out << "          if (!ret.Done)\n";
      out << std::format("            owner = Owner.C{};\n", i);
      out << "          reply(ret);\n";
      out << "        }\n";
      out << "      }\n";
      out << std::format("      on client{}.abort(): {{\n", i);
      out << std::format("        if (client{}.state.Error)\n", i);
      out << "          reply(Result.Error);\n";
      out << "        else\n";
      out << "          reply(Result.Success);\n";
      out << "      }\n";
      out << std::format("      on client{}.reset(): {{ reply(Result.Success); }}\n\n", i);
    }
    out << "    }\n\n";

    out << "    on resource.success(): {\n";
    out << "      succeeding = true;\n";
    out << "      defer () {\n";
    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("        if (owner.C{} || pending.C{})\n", i, i);
      out << std::format("          client{}.success();\n", i);
    }
    out << "\n";
    out << "        owner = Owner.None;\n";
    out << "        pending = Owner.None;\n";
    out << "        succeeding = false;\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    on resource.failure(): {\n";
    out << "      erroring = true;\n";
    out << "      defer () {\n";
    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("        if (owner.C{} || pending.C{})\n", i, i);
      out << std::format("          client{}.failure();\n", i);
    }
    out << "\n";
    out << "        pending = Owner.None;\n";
    out << "        erroring = false;\n";
    out << "      }\n";
    out << "    }\n\n";

    for (uint32_t i = 0; i < instances; ++i)
    {
      out << std::format("    [owner.C{}] {{\n", i);
      out << std::format("      on client{}.abort(): {{\n", i);
      out << "        if (erroring)\n";
      out << "          reply(Result.Error);\n";
      out << "        else if (succeeding)\n";
      out << "          reply(Result.Success);\n";
      out << "        else\n";
      out << "          reply(handleAbort());\n";
      out << "      }\n\n";

      out << std::format("      on client{}.reset(): {{\n", i);
      out << "        Result ret = resource.reset();\n";
      out << "        if (ret.Success)\n";
      out << "        {\n";
      out << "          owner = Owner.None;\n";
      out << "          pending = Owner.None;\n";
      out << "        }\n";
      out << "        reply(ret);\n";
      out << "      }\n\n";

      for (uint32_t j = 0; j < instances; ++j)
      {
        if (j == i)
          continue;

        out << std::format("      on client{}.abort(): {{\n", j);
        out << std::format("        if (client{}.state.Error)\n", j);
        out << "          reply(Result.Error);\n";
        out << std::format("        else if (client{}.state.Idle)\n", j);
        out << "          reply(Result.Success);\n";
        out << "        else\n";
        out << "          reply(Result.Running);\n";
        out << "      }\n\n";

        out << std::format("      on client{}.reset(): {{ reply(Result.Failure); }}\n\n", j);

        out << std::format("      on client{}.trigger(): {{\n", j);
        out << "        if (resource.state.Error) {\n";
        out << "          reply(Result.Failure);\n";
        out << "        } else {\n";
        out << std::format("          pending = Owner.C{};\n", j);
        out << "          reply(Result.Success);\n";
        out << "        }\n";
        out << "      }\n";
      }
      out << "    }\n";
    }

    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createConditionComponent(Model& model, const std::string& outdir, const std::string& name, SymbolId componentId)
{
  return createComponent(model, outdir, name, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    component.providesPorts.push_back({"api", PortProtocol::Condition});

    out << "import icondition.dzn;\n\n";
    out << std::format("component {} {{\n", component.name);
    out << "  provides icondition api;\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createCapabilityArmour(Model& model, const std::string& outdir, const std::string& capabilityName, bool hasAbort,
                                                const std::vector<std::string>& ports, SymbolId componentId)
{
  const auto componentName = std::format("{}_armour", capabilityName);
  return createComponent(model, outdir, componentName, componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    for (const auto& port : ports)
    {
      component.providesPorts.push_back({std::format("{}", port), PortProtocol::Action});
      component.providesPorts.push_back({std::format("r_{}", port), PortProtocol::External});
    }

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import iabort.dzn;\n";
    out << "import iexternal.dzn;\n";
    out << "\n";
    out << std::format("component {} {{\n", component.name);
    for (const auto& port : ports)
    {
      out << std::format("  provides iaction {};\n", port);
      out << std::format("  requires iexternal r_{};\n", port);
    }

    out << "\n";
    if (hasAbort)
    {
      out << "  provides iabort abort;\n";
      out << "\n";
    }

    out << "  behavior {\n";
    out << "    enum State { Idle";
    for (size_t i = 0; i < ports.size(); ++i)
      out << std::format(", Action{}, Aborting{}", i, i);

    out << ", Error };\n";
    out << "    State state = State.Idle;\n";
    out << "\n";
    out << "    [state.Idle] {\n";
    for (size_t i = 0; i < ports.size(); ++i)
    {
      const auto port = ports.at(i);
      out << std::format("      on {}.trigger(): {{\n", port);
      out << std::format("        Result ret = r_{}.trigger();\n", port);
      out << "\n";
      out << "        if (ret.Success)\n";
      out << std::format("          state = State.Action{};\n", i);
      out << "        else if (ret.Failure)\n";
      out << "          state = State.Error;\n";
      out << "\n";
      out << "        reply(ret);\n";
      out << "      }\n";
      out << "\n";
    }
    if (hasAbort)
    {
      out << "      on abort.abort(): {\n";
      out << "        reply(Result.Success);\n";
      out << "      }\n";
    }
    out << "    }\n";
    out << "\n";
    for (size_t i = 0; i < ports.size(); ++i)
    {
      const auto port = ports.at(i);
      out << std::format("    [state.Action{}] {{\n", i);
      out << std::format("      on r_{}.success(): {{\n", port);
      out << "        state = State.Idle;\n";
      out << std::format("        {}.success();\n", port);
      out << "      }\n";
      out << "\n";
      out << std::format("      on r_{}.failure(): {{\n", port);
      out << "        state = State.Error;\n";
      out << std::format("        {}.failure();\n", port);
      out << "      }\n";
      out << "\n";

      out << std::format("      on {}.abort(): {{\n", port);
      out << std::format("        Result ret = r_{}.abort();\n", port);
      out << "        if (ret.Failure)\n";
      out << "          state = State.Error;\n";
      out << "        else if (ret.Success)\n";
      out << "        {\n";
      out << "          state = State.Idle;\n";
      out << std::format("          {}.aborted();\n", port);
      out << "        }\n";
      out << "\n";
      out << "        reply(ret);\n";
      out << "      }\n";
      out << "\n";
      if (hasAbort)
      {
        out << "      on abort.abort(): {\n";
        out << std::format("        Result ret = r_{}.abort();\n", port);
        out << "        if (ret.Failure)\n";
        out << "        {\n";
        out << "          state = State.Error;\n";
        out << "          reply(Result.Failure);\n";
        out << "        }\n";
        out << "        else if (ret.Success)\n";
        out << "        {\n";
        out << std::format("          state = State.Aborting{};\n", i);
        out << std::format("          defer(state) {{ state = State.Idle; {}.aborted(); }}\n", port);
        out << "          reply(Result.Success);\n";
        out << "        }\n";
        out << "        else\n";
        out << "        {\n";
        out << "          reply(Result.Running);\n";
        out << "        }\n";
        out << "      }\n";
      }
      out << "    }\n";
      out << "\n";
      out << std::format("    [state.Aborting{}] {{\n", i);
      out << std::format("      on {}.abort(): {{\n"
                         "        {}.aborted();\n"
                         "        state = State.Idle;\n"
                         "        reply(Result.Success);\n"
                         "      }}\n",
                         port, port);
      out << "    }\n";
    }

    out << "    [state.Error] {\n";
    for (const auto& port : ports)
    {
      out << std::format("      on {}.reset(): {{\n", port);
      out << std::format("        Result ret = r_{}.reset();\n", port);
      out << "\n";
      out << "        if (ret.Success)\n";
      out << "          state = State.Idle;\n";
      out << "\n";
      out << "        reply(ret);\n";
      out << "      }\n";
      out << "\n";
      out << std::format("      on {}.abort(): {{\n", port);
      out << "        reply(Result.Error);\n";
      out << "      }\n";
      out << "\n";
    }
    if (hasAbort)
    {
      out << "      on abort.abort(): {\n";
      out << "        reply(Result.Failure);\n";
      out << "      }\n";
    }
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

Result<LibraryComponent> createFlowArmour(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "flow_armour", componentId, [&](LibraryComponent& component, std::ostringstream& out) {
    out << "import iaction.dzn;\n";
    out << "import iabort.dzn;\n\n";

    out << std::format("component {} {{\n", component.name);
    out << "  provides iaction api;\n";
    out << "  provides iabort abort;\n\n";

    out << "  requires iaction resource;\n\n";

    out << "  behaviour {\n";
    out << "    on api.trigger(): {\n";
    out << "      reply(resource.trigger());\n";
    out << "    }\n";
    out << "    on api.abort(): {\n";
    out << "      reply(resource.abort());\n";
    out << "    }\n";
    out << "    on api.reset(): {\n";
    out << "      reply(resource.reset());\n";
    out << "    }\n";
    out << "    on abort.abort(): {\n";
    out << "      reply(resource.abort());\n";
    out << "    }\n";
    out << "    on resource.success(): api.success();\n";
    out << "    on resource.failure(): api.failure();\n";
    out << "    on resource.aborted(): api.aborted();\n";
    out << "  }\n";
    out << "}\n";
  });
}

// ===========================================================================================================
// Helper components
VoidResult createAlarmComponent(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/alarm.dzn", outdir);
  std::ostringstream out;

  out << "import ialarm.dzn;\n\n";
  out << "component calarm\n";
  out << "{\n";
  out << "  provides ialarm api;\n";
  out << "}\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult createAlarmInterface(Model& model, const std::string& outdir)
{
  const auto path = std::format("{}/lib/ialarm.dzn", outdir);

  std::ostringstream out;
  out << "import types.dzn;\n\n";
  out << "interface ialarm\n";
  out << "{\n";
  out << "  in void set(int millis);\n";
  out << "  in void reset();\n";
  out << "  out void timeout();\n\n";
  out << "  behavior {\n";
  out << "    enum State { Idle, Running };\n";
  out << "    State state = State.Idle;\n";
  out << "    [state.Idle] {\n";
  out << "      on set: { state = State.Running; }\n";
  out << "      on reset: {}\n";
  out << "    }\n\n";
  out << "    [state.Running] {\n";
  out << "      on set: {}\n";
  out << "      on reset: { state = State.Idle; }\n";
  out << "      on optional: { timeout; state = State.Idle; }\n";
  out << "    }\n";
  out << "  }\n";
  out << "}\n";

  model.setGeneratedFile(path, out.str());

  return VoidResult();
}

// ===========================================================================================================
// Local helpers
void createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent)
{
  if (start >= instances)
  {
    if (fromIdle)
    {
      out << std::format("{}state = State.Idle;\n", indent);
    }
    else
    {
      out << std::format("{}api.success();\n", indent);
      out << std::format("{}state = State.Idle;\n", indent);
    }
  }
  else
  {
    out << std::format("{}ret = action{}.trigger();\n", indent, start);
    out << std::format("{}if (ret.Success) {{\n", indent);
    out << std::format("{}  state = State.Action{};\n", indent, start);
    out << std::format("{}}} else if (ret.Done) {{\n", indent);
    createSequenceDoneRecursion(fromIdle, start + 1, instances, out, indent + "  ");
    out << std::format("{}}} else if (ret.Failure) {{\n", indent);
    if (!fromIdle)
      out << std::format("{}  api.failure();\n", indent);

    out << std::format("{}  state = State.Error;\n", indent);
    out << std::format("{}}}\n", indent);
  }
}

void createParallelDoneRecursion(bool fromIdle, bool fromDone, uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent)
{
  if (start >= instances)
  {
    if (fromDone)
    {
      out << std::format("{}ret = Result.Done;\n", indent);
    }
    else if (fromIdle)
    {
      out << std::format("{}state = State.Running;\n", indent);
      out << std::format("{}ret = Result.Success;\n", indent);
    }
    else
    {
      out << std::format("{}api.success();\n", indent);
      out << std::format("{}state = State.Idle;\n", indent);
    }
  }
  else
  {
    out << std::format("{}ret = action{}.trigger();\n", indent, start);
    out << std::format("{}if (ret.Success) {{\n", indent);
    createParallelDoneRecursion(fromIdle, false, start + 1, instances, out, indent + "  ");
    out << std::format("{}}} else if (ret.Done) {{\n", indent);
    out << std::format("{}  completed = completed + 1;\n", indent);
    createParallelDoneRecursion(fromIdle, fromDone, start + 1, instances, out, indent + "  ");
    out << std::format("{}}} else if (ret.Failure) {{\n", indent);
    for (uint32_t i = 0; i < start && !fromDone; ++i)
      out << std::format("{}  Result abrt{} = action{}.abort();\n", indent, i, i);
    out << std::format("{}  state = State.Error;\n", indent);
    out << std::format("{}}}\n", indent);
  }
}

void createSelectorRecursion(uint32_t start, uint32_t instances, std::ostringstream& out, const std::string& indent)
{
  const uint32_t next = start + 1;
  out << std::format("{}ConditionResult c{} = condition{}.evaluate();\n", indent, start, start);
  out << std::format("{}if (c{}.True) {{\n", indent, start);
  out << std::format("{}  Result ret = flow{}.trigger();\n", indent, start);
  out << std::format("{}  if (ret.Success)\n", indent);
  out << std::format("{}    state = State.Flow{};\n", indent, start);
  out << std::format("{}  else if (ret.Failure)\n", indent);
  out << std::format("{}    state = State.Error;\n", indent);
  out << std::format("{}  reply(ret);\n", indent);
  out << std::format("{}}} else {{\n", indent);
  if (next < instances)
  {
    createSelectorRecursion(next, instances, out, indent + "  ");
  }
  else
  {
    out << std::format("{}  Result ret = flow{}.trigger();\n", indent, next);
    out << std::format("{}  if (ret.Success)\n", indent);
    out << std::format("{}    state = State.Flow{};\n", indent, next);
    out << std::format("{}  else if (ret.Failure)\n", indent);
    out << std::format("{}    state = State.Error;\n", indent);
    out << std::format("{}  reply(ret);\n", indent);
  }
  out << std::format("{}}}\n", indent);
}
}  // namespace koda::dezyne
