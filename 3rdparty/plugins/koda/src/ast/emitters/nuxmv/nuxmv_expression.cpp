#include "nuxmv_expression.h"

#include <sstream>
#include <type_traits>

namespace koda::nuxmv
{
namespace
{
std::string render(const PExpression& expression)
{
  if (!expression)
    return "FALSE";

  return std::visit(
      [](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, Expression::Literal>)
        {
          return value.value;
        }
        else if constexpr (std::is_same_v<T, Expression::Reference>)
        {
          return value.name;
        }
        else if constexpr (std::is_same_v<T, Expression::Unary>)
        {
          if (value.op == "X")
            return "X (" + render(value.operand) + ")";
          return value.op + "(" + render(value.operand) + ")";
        }
        else if constexpr (std::is_same_v<T, Expression::Binary>)
        {
          return "(" + render(value.lhs) + " " + value.op + " " + render(value.rhs) + ")";
        }
        else if constexpr (std::is_same_v<T, Expression::Case>)
        {
          std::ostringstream out;
          out << "case\n";
          for (const auto& branch : value.branches)
            out << "    " << render(branch.condition) << " : " << render(branch.value) << ";\n";
          out << "    TRUE : " << render(value.fallback) << ";\n";
          out << "  esac";
          return out.str();
        }

        return {};
      },
      expression->value);
}
}  // namespace

PExpression literal(std::string value)
{
  return std::make_shared<Expression>(Expression{Expression::Literal{std::move(value)}});
}

PExpression reference(std::string name)
{
  return std::make_shared<Expression>(Expression{Expression::Reference{std::move(name)}});
}

PExpression unary(std::string op, PExpression operand)
{
  return std::make_shared<Expression>(Expression{Expression::Unary{std::move(op), std::move(operand)}});
}

PExpression binary(PExpression lhs, std::string op, PExpression rhs)
{
  return std::make_shared<Expression>(Expression{Expression::Binary{std::move(lhs), std::move(op), std::move(rhs)}});
}

PExpression cases(std::vector<Expression::Case::Branch> branches, PExpression fallback)
{
  return std::make_shared<Expression>(Expression{Expression::Case{std::move(branches), std::move(fallback)}});
}

PExpression boolean(bool value)
{
  return literal(value ? "TRUE" : "FALSE");
}

PExpression integer(int value)
{
  return literal(std::to_string(value));
}

PExpression logicalNot(PExpression expression)
{
  return unary("!", std::move(expression));
}

PExpression logicalAnd(PExpression lhs, PExpression rhs)
{
  return binary(std::move(lhs), "&", std::move(rhs));
}

PExpression logicalOr(PExpression lhs, PExpression rhs)
{
  return binary(std::move(lhs), "|", std::move(rhs));
}

PExpression equal(PExpression lhs, PExpression rhs)
{
  return binary(std::move(lhs), "=", std::move(rhs));
}

PExpression notEqual(PExpression lhs, PExpression rhs)
{
  return binary(std::move(lhs), "!=", std::move(rhs));
}

PExpression next(PExpression expression)
{
  return unary("X", std::move(expression));
}

PExpression allOf(const std::vector<PExpression>& expressions)
{
  if (expressions.empty())
    return boolean(true);

  auto result = expressions.front();
  for (std::size_t i = 1; i < expressions.size(); ++i)
    result = logicalAnd(result, expressions[i]);
  return result;
}

PExpression anyOf(const std::vector<PExpression>& expressions)
{
  if (expressions.empty())
    return boolean(false);

  auto result = expressions.front();
  for (std::size_t i = 1; i < expressions.size(); ++i)
    result = logicalOr(result, expressions[i]);
  return result;
}

std::string toString(const PExpression& expression)
{
  return render(expression);
}

}  // namespace koda::nuxmv
