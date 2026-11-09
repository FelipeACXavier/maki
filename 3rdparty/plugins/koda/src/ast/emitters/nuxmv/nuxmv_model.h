#pragma once

#include <optional>
#include <string>
#include <vector>

#include "ast/ast.h"
#include "ast/symbol.h"
#include "nuxmv_expression.h"

namespace koda::nuxmv
{
enum class VariableKind
{
  Input,
  State
};

struct Provenance
{
  std::optional<koda::SymbolId> sourceSymbol;
  std::optional<Span> sourceSpan;
};

struct Variable
{
  VariableKind kind = VariableKind::State;
  std::string name;
  std::string domain;
  PExpression initial;
  Provenance origin;
};

// Most definitions are aliases. Complex, input-dependent control guards can
// instead be encoded as constrained Boolean inputs to avoid DEFINE expansion.
enum class DefinitionEncoding
{
  Macro,
  ConstrainedBooleanInput,
};

struct Definition
{
  std::string name;
  PExpression expression;
  Provenance origin;
  DefinitionEncoding encoding = DefinitionEncoding::Macro;
};

struct GuardedValue
{
  PExpression condition;
  PExpression value;
};

struct NextAssignment
{
  std::string variable;
  std::vector<GuardedValue> cases;
  PExpression fallback;
  Provenance origin;
};

struct TransitionConstraint
{
  PExpression expression;
  Provenance origin;
};

enum class PropertyKind
{
  Ltl,
  Ctl,
  Invariant,
};

// Verification obligations: never inserted as transition constraints.
struct Property
{
  std::string name;
  PExpression expression;
  Provenance origin;
  PropertyKind kind = PropertyKind::Ltl;
};

struct Behaviour
{
  std::string id;
  Provenance origin;

  PExpression trigger;
  PExpression abort;
  PExpression reset;
  PExpression error;

  PExpression triggerReply;
  PExpression abortReply;
  PExpression resetReply;
  PExpression outcome;

  std::vector<Variable> variables;
  std::vector<Definition> definitions;
  std::vector<NextAssignment> nextAssignments;
  std::vector<TransitionConstraint> constraints;
  std::vector<Property> expectations;

  // Conservative upper bounds on possible control-flow outcomes. Until a
  // composite compiler has been annotated, TRUE means "not yet ruled out".
  // Only a compiler that knows an outcome is impossible should set it FALSE.
  // This prevents optimization from deleting legitimate execution paths.
  bool mayReturnDone = true;   // existing synchronous RESULT_DONE capability
  bool mayAccept = true;       // RESULT_SUCCESS (async trigger accepted)
  bool mayReject = true;       // RESULT_FAILURE (trigger rejected)
  bool mayEmitSuccess = true;  // EVENT_SUCCESS from an accepted invocation
  bool mayEmitFailure = true;  // EVENT_FAILURE from an accepted invocation
  bool mayEmitAborted = true;  // EVENT_ABORTED from an accepted invocation
};

class Model
{
public:
  void clear();

  void setName(std::string name);
  const std::string& name() const;

  void setOutputPath(std::string path);
  const std::string& outputPath() const;

  void addVariable(Variable variable);
  void addDefinition(Definition definition);
  void addNextAssignment(NextAssignment assignment);
  void addConstraint(TransitionConstraint constraint);
  void addProperty(Property property);
  void addBehaviour(Behaviour behaviour);

  const std::vector<Variable>& variables() const;
  const std::vector<Definition>& definitions() const;
  const std::vector<NextAssignment>& nextAssignments() const;
  const std::vector<TransitionConstraint>& constraints() const;
  const std::vector<Property>& properties() const;

private:
  std::string mName;
  std::string mOutputPath;
  std::vector<Variable> mVariables;
  std::vector<Definition> mDefinitions;
  std::vector<NextAssignment> mNextAssignments;
  std::vector<TransitionConstraint> mConstraints;
  std::vector<Property> mProperties;
};

}  // namespace koda::nuxmv
