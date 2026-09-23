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

struct Definition
{
  std::string name;
  PExpression expression;
  Provenance origin;
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

struct Property
{
  std::string name;
  PExpression expression;
  Provenance origin;
};

struct Behaviour
{
  std::string id;
  Provenance origin;

  PExpression trigger;
  PExpression abort;
  PExpression reset;

  PExpression triggerReply;
  PExpression abortReply;
  PExpression resetReply;
  PExpression outcome;

  std::vector<Variable> variables;
  std::vector<Definition> definitions;
  std::vector<NextAssignment> nextAssignments;
  std::vector<TransitionConstraint> constraints;
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
