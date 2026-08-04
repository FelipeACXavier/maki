#include "dezyne_library_emitter.h"

#include <format>

namespace koda::dezyne
{

// ===========================================================================================================
// Main orchestration components
VoidResult createSequenceComponent(uint32_t instances, std::ostringstream& out)
{
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

  return VoidResult();
}

VoidResult createParallelComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createEveryComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createWithinComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createRepeatComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createSignalHandlerComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createSignalContinueComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createAbortHandlerComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

VoidResult createErrorHandlerComponent(uint32_t instances, std::ostringstream& out)
{
  return VoidResult();
}

// ===========================================================================================================
// Helper components
VoidResult createAlarmComponent(std::ostringstream& out)
{
  out << "import ialarm.dzn;\n\n";

  out << "component calarm\n";
  out << "{\n";
  out << "  provides ialarm api;\n";
  out << "}\n";

  return VoidResult();
}

VoidResult createAlarmInterface(std::ostringstream& out)
{
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

  return VoidResult();
}

VoidResult createActionArbiterComponent(uint32_t instances, std::ostringstream& out)
{
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

}  // namespace koda::dezyne