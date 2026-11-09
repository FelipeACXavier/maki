#include "nuxmv_writer.h"

#include <filesystem>
#include <fstream>
#include <ostream>

#include "logging.h"

namespace koda::nuxmv
{
VoidResult Writer::write(const Model& model, bool dryRun)
{
  if (dryRun)
    return VoidResult();

  const auto path = std::filesystem::path(model.outputPath());
  if (path.empty())
    return VoidResult::Failed("nuXmv model has no output path");

  LOG_DEBUG("Writing model {} to {}", model.name(), path.native());

  if (!path.parent_path().empty())
    std::filesystem::create_directories(path.parent_path());

  std::ofstream stream(path);
  if (!stream.is_open())
    return VoidResult::Failed("Failed to open: " + path.string());

  auto result = write(model, stream);
  if (result.IsSuccess())
    mGeneratedFiles.push_back(path.string());

  return result;
}

VoidResult Writer::write(const Model& model, std::ostream& stream) const
{
  stream << "-- Generated from KODA. Semantic model: " << model.name() << "\n";
  stream << "MODULE main\n\n";

  bool hasInputs = false;
  for (const auto& variable : model.variables())
    hasInputs |= variable.kind == VariableKind::Input;
  for (const auto& definition : model.definitions())
    hasInputs |= definition.encoding == DefinitionEncoding::ConstrainedBooleanInput;

  if (hasInputs)
  {
    stream << "IVAR\n";
    for (const auto& variable : model.variables())
      if (variable.kind == VariableKind::Input)
        writeVariable(stream, variable);
    for (const auto& definition : model.definitions())
      if (definition.encoding == DefinitionEncoding::ConstrainedBooleanInput)
        stream << "  " << definition.name << " : boolean;\n";
    stream << "\n";
  }

  bool hasState = false;
  for (const auto& variable : model.variables())
    hasState |= variable.kind == VariableKind::State;

  if (hasState)
  {
    stream << "VAR\n";
    for (const auto& variable : model.variables())
      if (variable.kind == VariableKind::State)
        writeVariable(stream, variable);
    stream << "\n";
  }

  stream << "DEFINE\n";
  writeConstants(stream);
  for (const auto& definition : model.definitions())
    if (definition.encoding == DefinitionEncoding::Macro)
      stream << "  " << definition.name << " := " << toString(definition.expression) << ";\n";
  stream << "\n";

  if (hasState)
  {
    stream << "ASSIGN\n";
    for (const auto& variable : model.variables())
      if (variable.kind == VariableKind::State && variable.initial)
        stream << "  init(" << variable.name << ") := " << toString(variable.initial) << ";\n";
    for (const auto& assignment : model.nextAssignments())
      writeNext(stream, assignment);
    stream << "\n";
  }

  for (const auto& constraint : model.constraints())
    stream << "TRANS " << toString(constraint.expression) << ";\n";

  // A constrained Boolean IVAR is *not* an unconstrained environment input.
  // The equality ties its current-step value to the original guard, including
  // current-step capability inputs. Current-state ASSIGN is invalid here.
  bool hasConstrainedInputs = false;
  for (const auto& definition : model.definitions())
    if (definition.encoding == DefinitionEncoding::ConstrainedBooleanInput)
    {
      stream << "TRANS (" << definition.name << " = (" << toString(definition.expression) << "));\n";
      hasConstrainedInputs = true;
    }

  if (!model.constraints().empty() || hasConstrainedInputs)
    stream << "\n";

  for (const auto& property : model.properties())
  {
    const char* keyword = "LTLSPEC";
    switch (property.kind)
    {
      case PropertyKind::Ltl:
        keyword = "LTLSPEC";
        break;
      case PropertyKind::Ctl:
        keyword = "CTLSPEC";
        break;
      case PropertyKind::Invariant:
        keyword = "INVARSPEC";
        break;
    }
    stream << keyword << " NAME " << property.name << " := " << toString(property.expression) << ";\n";
  }

  return {};
}

const std::vector<std::string>& Writer::generatedFiles() const
{
  return mGeneratedFiles;
}

void Writer::clear()
{
  mGeneratedFiles.clear();
}

void Writer::writeConstants(std::ostream& stream) const
{
  stream << "  RESULT_NONE    := 0;\n"
         << "  RESULT_SUCCESS := 1;\n"
         << "  RESULT_FAILURE := 2;\n"
         << "  RESULT_RUNNING := 3;\n"
         << "  RESULT_ERROR   := 4;\n"
         << "  RESULT_DONE    := 5;\n\n"
         << "  CMD_NONE       := 0;\n"
         << "  CMD_TRIGGER    := 1;\n"
         << "  CMD_ABORT      := 2;\n"
         << "  CMD_RESET      := 3;\n\n"
         << "  EVENT_NONE     := 0;\n"
         << "  EVENT_SUCCESS  := 1;\n"
         << "  EVENT_FAILURE  := 2;\n"
         << "  EVENT_ABORTED  := 3;\n"
         << "\n";
}

void Writer::writeVariable(std::ostream& stream, const Variable& variable) const
{
  stream << "  " << variable.name << " : " << variable.domain << ";\n";
}

void Writer::writeNext(std::ostream& stream, const NextAssignment& assignment) const
{
  stream << "  next(" << assignment.variable << ") :=\n";
  stream << "    case\n";
  for (const auto& item : assignment.cases)
    stream << "      " << toString(item.condition) << " : " << toString(item.value) << ";\n";
  stream << "      TRUE : " << toString(assignment.fallback) << ";\n";
  stream << "    esac;\n";
}

}  // namespace koda::nuxmv
