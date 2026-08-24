#include "dezyne_library_emitter.h"

#include <format>

namespace koda::dezyne
{

VoidResult createComponent(Model& model, const std::string& outdir, const std::string& name,
                           std::function<void(const std::string& componentName, const std::string& path, std::ostringstream& out)> callback)
{
  const auto componentName = std::format("c{}", name);
  const auto path = std::format("{}/lib/{}.dzn", outdir, name);

  std::ostringstream out;
  callback(componentName, path, out);

  model.setGeneratedFile(path, out.str());

  return VoidResult();
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
  out << "    Pending,\n";
  out << "    Error\n";
  out << "  };\n\n";

  out << "  extern float $float$;\n";
  out << "  extern int $int$;\n";
  out << "  extern boolean $bool$;\n";

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
  out << "  out void failure();\n\n";

  out << "  behavior {\n";
  out << "    enum State { Idle, Running, Error };\n";
  out << "    State state = State.Idle;\n\n";

  out << "    [state.Idle] {\n";
  out << "      on trigger: { state = State.Running; reply(Result.Success); }\n";
  out << "      on trigger: { state = State.Error; reply(Result.Failure); }\n";
  out << "      on trigger: { reply(Result.Done); }\n";
  out << "    }\n\n";

  out << "    [state.Running] {\n";
  out << "      on abort: { state = State.Idle; reply(Result.Success); }\n";
  out << "      on abort: { state = State.Error; reply(Result.Failure); }\n";
  out << "      on abort: { reply(Result.Running); }\n";
  out << "      on optional: { state = State.Idle; success; }\n";
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

// ===========================================================================================================
// Main orchestration components
VoidResult createSequenceComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  auto numberedName = std::format("sequence{}", instances);
  return createComponent(model, outdir, numberedName, [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto component = model.declareComponent(name, path, {componentId}, true, componentId);
    model.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action);

    for (std::uint32_t i = 0; i < instances; ++i)
      model.declarePort(component, std::format("action{}", i), PortDirection::Requires, PortProtocol::Action);

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n\n";

    out << std::format("component csequence{} {{\n", instances);
    out << "  provides iaction api;\n\n";

    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("  requires iaction action{};\n", i);

    out << "\n";

    out << "  behaviour {\n";
    out << "    enum State { Idle, ";
    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("Action{}, ", i);
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

      out << "      on api.abort(): {\n";
      out << std::format("        Result ret = action{}.abort();\n", i);
      out << "        if (ret.Success)\n";
      out << "          state = State.Idle;\n";
      out << "        else if (ret.Failure)\n";
      out << "          state = State.Error;\n";
      out << "        reply(ret);\n";
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

VoidResult createParallelComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  auto numberedName = std::format("parallel{}", instances);
  return createComponent(model, outdir, numberedName, [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto component = model.declareComponent(name, path, {componentId}, true, componentId);
    model.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action);

    for (std::uint32_t i = 0; i < instances; ++i)
      model.declarePort(component, std::format("action{}", i), PortDirection::Requires, PortProtocol::Action);

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";

    out << std::format("component {} {{\n", name);
    out << "  provides iaction api;\n";

    for (uint32_t i = 0; i < instances; ++i)
      out << std::format("  requires iaction action{};\n", i);

    out << "  behaviour {\n";
    out << "    enum State { Idle, Running, Error };\n";
    out << std::format("    subint Completed {{0..{}}};\n", instances);

    out << "    State state = State.Idle;\n";
    out << "    Completed completed = 0;\n";

    // ------------------------------------------------------------------
    // Idle state
    out << "    [state.Idle] {\n";
    out << "      on api.trigger(): {\n";
    out << "        complete = 0;\n";
    out << "        Result ret = action0.trigger();\n";
    out << "        if (ret.Success) {\n";
    createParallelDoneRecursion(true, false, 1, instances, out, "          ");
    out << "        } else if (ret.Done) {\n";
    out << "          completed = complete + 1;\n";
    createParallelDoneRecursion(true, true, 1, instances, out, "          ");
    out << "        } else {\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "        reply(ret);\n";
    out << "      }\n";
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
    out << "          state = State.Idle;\n";
    out << "          reply(Result.Success);\n";
    out << std::format("        }} else if ({}) {{\n", disjunction);
    out << "          state = State.Error;\n";
    out << "          reply(Result.Failure);\n";
    out << "        } else {\n";
    out << "          reply(Result.Running);\n";
    out << "        }\n";
    out << "      }\n";
    out << "    }\n";

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
    }
    out << "  }\n";
    out << "}\n";
  });
}

VoidResult createEveryComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "every", [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto component = model.declareComponent(name, path, {componentId}, true, componentId);
    model.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action);
    model.declarePort(component, "action", PortDirection::Requires, PortProtocol::Action);

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import ialarm.dzn;\n";
    out << "component cevery {\n";
    out << "  provides iaction api;\n";
    out << "  requires ialarm alarm;\n";
    out << "  requires iaction action;\n";
    out << "  behaviour {\n";
    out << "    enum State { State0, State1, State2, Error };\n";
    out << "    State state = State.State0;\n";
    out << "    [state.State0] {\n";
    out << "      on api.trigger(): {\n";
    out << "        alarm.set($30$);\n";
    out << "        state = State.State1;\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n";
    out << "      // on api.abort(): { reply(Result.Success); }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.State1] {\n";
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
    out << "        state = State.State0;\n";
    out << "        reply(Result.Success);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
    out << "    [state.State2] {\n";
    out << "      on action1.success(): {\n";
    out << "        alarm.set($30$);\n";
    out << "        state = State.State1;\n";
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
    out << "          state = State.State0;\n";
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
    out << "          state = State.State0;\n";
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

VoidResult createWithinComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "within", [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto component = model.declareComponent(name, path, {componentId}, true, componentId);
    model.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action);
    model.declarePort(component, "action", PortDirection::Requires, PortProtocol::Action);
    model.declarePort(component, "handler", PortDirection::Requires, PortProtocol::Action);

    out << "import types.dzn;\n";
    out << "import iaction.dzn;\n";
    out << "import ialarm.dzn;\n\n";

    out << std::format("component {} {{\n", name);
    out << "  provides iaction api;\n";
    out << "  requires ialarm alarm;\n";
    out << "  requires iaction action1;\n";
    out << "  requires iaction action2;\n\n";

    out << "  behaviour {\n";
    out << "    enum State { State0, State1, Waiting, State2, Error };\n";
    out << "    State state = State.State0;\n\n";

    out << "    [state.State0] {\n";
    out << "      on api.trigger(): {\n";
    out << "        Result triggered = action1.trigger();\n";
    out << "        if (triggered.Success) {\n";
    out << "          alarm.set($30$);\n";
    out << "          state = State.State1;\n";
    out << "        } else if (triggered.Failure) {\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "        reply(triggered);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.State1] {\n";
    out << "      on alarm.timeout(): {\n";
    out << "        Result aborted = action1.abort();\n";
    out << "        if (aborted.Success) {\n";
    out << "          Result handled = action2.trigger();\n";
    out << "          if (handled.Success) {\n";
    out << "            state = State.State2;\n";
    out << "          } else if (handled.Failure) {\n";
    out << "            api.failure();\n";
    out << "            state = State.Error;\n";
    out << "          } else {\n";
    out << "            api.success();\n";
    out << "            state = State.State0;\n";
    out << "          }\n";
    out << "        } else if (aborted.Running) {\n";
    out << "          state = State.Waiting;\n";
    out << "        } else {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on action1.success(): {\n";
    out << "        alarm.reset();\n";
    out << "        api.success();\n";
    out << "        state = State.State0;\n";
    out << "      }\n";
    out << "      on action1.failure(): {\n";
    out << "        alarm.reset();\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        alarm.reset();\n";
    out << "        Result res = action1.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.State0;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Waiting] {\n";
    out << "      on action1.success(): {\n";
    out << "        Result handled = action2.trigger();\n";
    out << "        if (handled.Success) {\n";
    out << "          state = State.State2;\n";
    out << "        } else if (handled.Failure) {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        } else {\n";
    out << "          api.success();\n";
    out << "          state = State.State0;\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on action1.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result res = action1.abort();\n";
    out << "        if (res.Success)\n";
    out << "          state = State.State0;\n";
    out << "        else if (res.Failure)\n";
    out << "          state = State.Error;\n";
    out << "        reply(res);\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.State2] {\n";
    out << "      on action2.success(): {\n";
    out << "        api.success();\n";
    out << "        state = State.State0;\n";
    out << "      }\n";
    out << "      on action2.failure(): {\n";
    out << "        api.failure();\n";
    out << "        state = State.Error;\n";
    out << "      }\n";
    out << "      on api.abort(): {\n";
    out << "        Result ret1 = Result.Success;\n";
    out << "        Result ret2 = Result.Success;\n";
    out << "        if (action1.state.Running) {\n";
    out << "          ret1 = action1.abort();\n";
    out << "        }\n";
    out << "        if (action2.state.Running) {\n";
    out << "          ret2 = action2.abort();\n";
    out << "        }\n";
    out << "        if (ret1.Success && ret2.Success) {\n";
    out << "          state = State.State0;\n";
    out << "          reply(Result.Success);\n";
    out << "        } else if (ret1.Failure || ret2.Failure) {\n";
    out << "          state = State.Error;\n";
    out << "          reply(Result.Failure);\n";
    out << "        } else {\n";
    out << "          reply(Result.Running);\n";
    out << "        }\n";
    out << "      }\n";
    out << "    }\n\n";

    out << "    [state.Error] {\n";
    out << "      on api.reset(): {\n";
    out << "        Result ret1 = Result.Success;\n";
    out << "        Result ret2 = Result.Success;\n";
    out << "        if (action1.state.Error) {\n";
    out << "          ret1 = action1.reset();\n";
    out << "        } else if (action1.state.Running) {\n";
    out << "          ret1 = action1.abort();\n";
    out << "        }\n";
    out << "        if (action2.state.Error) {\n";
    out << "          ret2 = action2.reset();\n";
    out << "        }\n";
    out << "        if (ret1.Success && ret2.Success) {\n";
    out << "          state = State.State0;\n";
    out << "          reply(Result.Success);\n";
    out << "        } else {\n";
    out << "          reply(Result.Failure);\n";
    out << "        }\n";
    out << "      }\n";
    out << "      on api.abort(): { reply(Result.Error); }\n";
    out << "      on action1.success(): {}\n";
    out << "      on action1.failure(): {}\n";
    out << "    }\n";
    out << "  }\n";
    out << "}\n";
  });
}

VoidResult createRepeatComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "repeat", [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto repeat = model.declareComponent(name, path, {componentId}, true, componentId);

    model.declarePort(repeat, "api", PortDirection::Provides, PortProtocol::Action);
    model.declarePort(repeat, "action", PortDirection::Requires, PortProtocol::Action);

    out << "  import types.dzn;\n";
    out << "  import iaction.dzn;\n";
    out << "  component crepeat {\n";
    out << "    provides iaction api;\n";
    out << "    requires iaction action;\n\n";

    out << "    behaviour {\n";
    out << "      enum State { Idle, Running, Error };\n";
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
    out << "        }\n";
    out << "  \n";
    out << "        on action.failure(): {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): {\n";
    out << "          Result res = action.abort();\n";
    out << "          if (res.Success)\n";
    out << "            state = State.Idle;\n";
    out << "          else if (res.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res);\n";
    out << "        }\n";
    out << "      }\n";
    out << "  \n";
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

  return VoidResult();
}

VoidResult createSignalHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return VoidResult();
}

VoidResult createSignalContinueComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return VoidResult();
}

VoidResult createAbortHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "abort_handler", [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto component = model.declareComponent(name, path, {componentId}, true, componentId);
    model.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action);
    model.declarePort(component, "action", PortDirection::Requires, PortProtocol::Action);
    model.declarePort(component, "handler", PortDirection::Requires, PortProtocol::Action);

    out << "  import types.dzn;\n";
    out << "  import iaction.dzn;\n";
    out << "  import isignal.dzn;\n";
    out << "  \n";
    out << "  component cabort_handler {\n";
    out << "    provides iaction api;\n";
    out << "  \n";
    out << "    requires iaction action;\n";
    out << "    requires iaction handler;\n";
    out << "  \n";
    out << "    behaviour {\n";
    out << "      enum State { State0, State1, State2, Error };\n";
    out << "      State state = State.State0;\n";
    out << "  \n";
    out << "      [state.State0] {\n";
    out << "        on api.trigger(): {\n";
    out << "          Result res1 = action.trigger();\n";
    out << "          if (res1.Success)\n";
    out << "            state = State.State1;\n";
    out << "          else if (res1.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res1);\n";
    out << "        }\n";
    out << "  \n";
    out << "        // on api.abort(): { reply(Result.Success); }\n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.State1] {\n";
    out << "        on action.failure(): {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on action.success(): {\n";
    out << "          api.success();\n";
    out << "          state = State.State0;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): {\n";
    out << "          Result aborted = action.abort();\n";
    out << "          if (aborted.Success) {\n";
    out << "            Result handled = handler.trigger();\n";
    out << "            if (handled.Success) {\n";
    out << "              state = State.State2;\n";
    out << "              reply(Result.Running);\n";
    out << "            } else if (handled.Failure) {\n";
    out << "              state = State.Error;\n";
    out << "              reply(handled);\n";
    out << "            } else {\n";
    out << "              state = State.State0;\n";
    out << "              reply(Result.Success);\n";
    out << "            }\n";
    out << "          } else if (aborted.Failure) {\n";
    out << "            state = State.Error;\n";
    out << "            reply(aborted);\n";
    out << "          } else {\n";
    out << "            reply (aborted);\n";
    out << "          }\n";
    out << "        }\n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.State2] {\n";
    out << "        on handler.success(): {\n";
    out << "          api.success();\n";
    out << "          state = State.State0;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on handler.failure(): {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): {\n";
    out << "          Result res = handler.abort();\n";
    out << "          if (res.Success)\n";
    out << "            state = State.State0;\n";
    out << "          else if (res.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res);\n";
    out << "        }\n";
    out << "  \n";
    out << "        // TODO(felaze): Is this correct?\n";
    out << "        on action.success(): {}\n";
    out << "        on action.failure(): {}\n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.Error] {\n";
    out << "        on api.reset(): {\n";
    out << "          Result ret1 = Result.Success;\n";
    out << "          Result ret2 = Result.Success;\n";
    out << "  \n";
    out << "          if (action.state.Error) {\n";
    out << "            ret1 = action.reset();\n";
    out << "          }\n";
    out << "  \n";
    out << "          if (handler.state.Error) {\n";
    out << "            ret2 = handler.reset();\n";
    out << "          }\n";
    out << "  \n";
    out << "          if (ret1.Success && ret2.Success) {\n";
    out << "            state = State.State0;\n";
    out << "            reply(Result.Success);\n";
    out << "          } else {\n";
    out << "            reply(Result.Failure);\n";
    out << "          }\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): { reply(Result.Error); }\n";
    out << "  \n";
    out << "        on action.success(): {}\n";
    out << "        on action.failure(): {}\n";
    out << "        on handler.success(): {}\n";
    out << "        on handler.failure(): {}\n";
    out << "      }\n";
    out << "    }\n";
    out << "  }\n";
  });
}

VoidResult createErrorHandlerComponent(Model& model, const std::string& outdir, SymbolId componentId)
{
  return createComponent(model, outdir, "error_handler", [&](const std::string& name, const std::string& path, std::ostringstream& out) {
    const auto component = model.declareComponent(name, path, {componentId}, true, componentId);
    model.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action);
    model.declarePort(component, "action", PortDirection::Requires, PortProtocol::Action);
    model.declarePort(component, "handler", PortDirection::Requires, PortProtocol::Action);

    out << "  import types.dzn;\n";
    out << "  import iaction.dzn;\n";
    out << "  \n";
    out << "  component cerror_handler {\n";
    out << "    provides iaction api;\n";
    out << "  \n";
    out << "    requires iaction action;\n";
    out << "    requires iaction handler;\n";
    out << "  \n";
    out << "    behaviour {\n";
    out << "      enum State { Idle, Action1, Action2, Error };\n";
    out << "      State state = State.Idle;\n";
    out << "  \n";
    out << "      [state.Idle] {\n";
    out << "        on api.trigger(): {\n";
    out << "          Result res1 = action.trigger();\n";
    out << "          if (res1.Success)\n";
    out << "            state = State.Action1;\n";
    out << "          else if (res1.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res1);\n";
    out << "        }\n";
    out << "  \n";
    out << "        // on api.abort(): { reply(Result.Success); }\n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.Action1] {\n";
    out << "        on action.failure(): {\n";
    out << "          // Does this make sense? Shouldn't the error handler keep the error and still do something?\n";
    out << "          Result reset = action.reset();\n";
    out << "          if (reset.Success) {\n";
    out << "            Result res = handler.trigger();\n";
    out << "            if (res.Success) {\n";
    out << "              state = State.Action2;\n";
    out << "            } else if (res.Done) {\n";
    out << "              api.success();\n";
    out << "              state = State.Idle;\n";
    out << "            } else {\n";
    out << "              api.failure();\n";
    out << "              state = State.Error;\n";
    out << "            }\n";
    out << "          } else {\n";
    out << "            api.failure();\n";
    out << "            state = State.Error;\n";
    out << "          }\n";
    out << "        }\n";
    out << "  \n";
    out << "        on action.success(): {\n";
    out << "          api.success();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): {\n";
    out << "          Result res = action.abort();\n";
    out << "          if (res.Success)\n";
    out << "            state = State.Idle;\n";
    out << "          else if (res.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res);\n";
    out << "        }\n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.Action2] {\n";
    out << "        on handler.success(): {\n";
    out << "          api.success();\n";
    out << "          state = State.Idle;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on handler.failure(): {\n";
    out << "          api.failure();\n";
    out << "          state = State.Error;\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): {\n";
    out << "          Result res = handler.abort();\n";
    out << "          if (res.Success)\n";
    out << "            state = State.Idle;\n";
    out << "          else if (res.Failure)\n";
    out << "            state = State.Error;\n";
    out << "  \n";
    out << "          reply(res);\n";
    out << "        }\n";
    out << "      }\n";
    out << "  \n";
    out << "      [state.Error] {\n";
    out << "        on api.reset(): {\n";
    out << "          Result ret1 = Result.Success;\n";
    out << "          Result ret2 = Result.Success;\n";
    out << "  \n";
    out << "          if (action.state.Error) {\n";
    out << "            ret1 = action.reset();\n";
    out << "          }\n";
    out << "  \n";
    out << "          if (handler.state.Error) {\n";
    out << "            ret2 = handler.reset();\n";
    out << "          }\n";
    out << "  \n";
    out << "          if (ret1.Success && ret2.Success) {\n";
    out << "            state = State.Idle;\n";
    out << "            reply(Result.Success);\n";
    out << "          } else {\n";
    out << "            reply(Result.Failure);\n";
    out << "          }\n";
    out << "        }\n";
    out << "  \n";
    out << "        on api.abort(): { reply(Result.Error); }\n";
    out << "      }\n";
    out << "    }\n";
    out << "  }\n";
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

VoidResult createActionArbiterComponent(Model& model, const std::string& outdir, uint32_t instances, SymbolId componentId)
{
  const auto name = std::format("caction_arbiter{}", instances);
  const auto path = std::format("{}/lib/action_arbiter{}.dzn", outdir, instances);
  const auto arbiter = model.declareComponent(name, path, {componentId}, true, componentId);

  for (std::uint32_t i = 0; i < instances; ++i)
    model.declarePort(arbiter, std::format("client{}", i), PortDirection::Provides, PortProtocol::Action, {componentId});

  model.declarePort(arbiter, "resource", PortDirection::Requires, PortProtocol::Action, {componentId});

  std::ostringstream out;
  out << "import types.dzn;\n";
  out << "import iaction.dzn;\n\n";

  out << std::format("component caction_arbiter{} {{\n", instances);
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
    for (uint32_t i = 0; i < start; ++i)
      out << std::format("{}  Result abrt{} = action{}.abort();\n", indent, i, i);
    out << std::format("{}  state = State.Error;\n", indent);
    out << std::format("{}}}\n", indent);
  }
}

}  // namespace koda::dezyne
