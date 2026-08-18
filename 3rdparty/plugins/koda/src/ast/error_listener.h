#pragma once

#include <antlr4-runtime.h>

class CollectingErrorListener : public antlr4::BaseErrorListener
{
public:
  std::vector<std::string> errors;

  void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string& msg,
                   std::exception_ptr e) override
  {
    errors.push_back("line " + std::to_string(line) + ":" + std::to_string(charPositionInLine) + " " + msg);
  }

  void addError(size_t line, size_t charPositionInLine, const std::string& msg)
  {
    errors.push_back("line " + std::to_string(line) + ":" + std::to_string(charPositionInLine) + " " + msg);
  }

  void addError(const antlr4::ParserRuleContext* ctx, const std::string& msg)
  {
    if (ctx && ctx->getStart())
      addError(ctx->getStart()->getLine(), ctx->getStart()->getCharPositionInLine(), msg);
    else
      errors.push_back(msg);
  }

  bool hasErrors() const
  {
    return !errors.empty();
  }
};