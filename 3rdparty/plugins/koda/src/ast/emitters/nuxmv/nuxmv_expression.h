#pragma once

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace koda::nuxmv
{
struct Expression;
using PExpression = std::shared_ptr<const Expression>;

struct Expression
{
  struct Literal
  {
    std::string value;
  };

  struct Reference
  {
    std::string name;
  };

  struct Unary
  {
    std::string op;
    PExpression operand;
  };

  struct Binary
  {
    PExpression lhs;
    std::string op;
    PExpression rhs;
  };

  struct Case
  {
    struct Branch
    {
      PExpression condition;
      PExpression value;
    };

    std::vector<Branch> branches;
    PExpression fallback;
  };

  using Value = std::variant<Literal, Reference, Unary, Binary, Case>;
  Value value;
};

PExpression literal(std::string value);
PExpression reference(std::string name);
PExpression unary(std::string op, PExpression operand);
PExpression binary(PExpression lhs, std::string op, PExpression rhs);
PExpression cases(std::vector<Expression::Case::Branch> branches, PExpression fallback);

PExpression boolean(bool value);
PExpression integer(int value);

PExpression logicalNot(PExpression expression);
PExpression logicalAnd(PExpression lhs, PExpression rhs);
PExpression logicalOr(PExpression lhs, PExpression rhs);
PExpression equal(PExpression lhs, PExpression rhs);
PExpression notEqual(PExpression lhs, PExpression rhs);
PExpression next(PExpression expression);

PExpression allOf(const std::vector<PExpression>& expressions);
PExpression anyOf(const std::vector<PExpression>& expressions);

std::string toString(const PExpression& expression);

}  // namespace koda::nuxmv
