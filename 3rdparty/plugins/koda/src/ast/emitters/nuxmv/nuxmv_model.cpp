#include "nuxmv_model.h"

#include <utility>

namespace koda::nuxmv
{
void Model::clear()
{
  mName.clear();
  mOutputPath.clear();
  mVariables.clear();
  mDefinitions.clear();
  mNextAssignments.clear();
  mConstraints.clear();
  mProperties.clear();
}

void Model::setName(std::string name) { mName = std::move(name); }
const std::string& Model::name() const { return mName; }

void Model::setOutputPath(std::string path) { mOutputPath = std::move(path); }
const std::string& Model::outputPath() const { return mOutputPath; }

void Model::addVariable(Variable variable) { mVariables.push_back(std::move(variable)); }
void Model::addDefinition(Definition definition) { mDefinitions.push_back(std::move(definition)); }
void Model::addNextAssignment(NextAssignment assignment) { mNextAssignments.push_back(std::move(assignment)); }
void Model::addConstraint(TransitionConstraint constraint) { mConstraints.push_back(std::move(constraint)); }
void Model::addProperty(Property property) { mProperties.push_back(std::move(property)); }

void Model::addBehaviour(Behaviour behaviour)
{
  for (auto& variable : behaviour.variables)
    addVariable(std::move(variable));
  for (auto& definition : behaviour.definitions)
    addDefinition(std::move(definition));
  for (auto& assignment : behaviour.nextAssignments)
    addNextAssignment(std::move(assignment));
  for (auto& constraint : behaviour.constraints)
    addConstraint(std::move(constraint));
}

const std::vector<Variable>& Model::variables() const { return mVariables; }
const std::vector<Definition>& Model::definitions() const { return mDefinitions; }
const std::vector<NextAssignment>& Model::nextAssignments() const { return mNextAssignments; }
const std::vector<TransitionConstraint>& Model::constraints() const { return mConstraints; }
const std::vector<Property>& Model::properties() const { return mProperties; }

}  // namespace koda::nuxmv
