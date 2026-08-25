#pragma once

// Fantastic clash between QT emit and antlr...
#ifdef emit
#pragma push_macro("emit")
#undef emit
#define KODA_RESTORE_QT_EMIT
#endif

#include <antlr4-runtime.h>

#ifdef KODA_RESTORE_QT_EMIT
#pragma pop_macro("emit")
#undef KODA_RESTORE_QT_EMIT
#endif

namespace koda
{
struct Error
{
  std::string nodeId;
  std::string flowId;
  std::string message;
};

class CollectingErrorListener : public antlr4::BaseErrorListener
{
public:
  std::vector<Error> mErrors;
  std::vector<std::string> errors;

  void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string& msg,
                   std::exception_ptr e) override
  {
    errors.push_back("line " + std::to_string(line) + ":" + std::to_string(charPositionInLine) + " " + msg);
  }

  void addError(const std::string& nodeId, const std::string& flowId, const std::string& msg)
  {
    mErrors.push_back({.nodeId = nodeId, .flowId = flowId, .message = msg});
  }

  void addError(const std::string& nodeId, const std::string& msg)
  {
    mErrors.push_back({.nodeId = nodeId, .flowId = std::string(), .message = msg});
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
    return !(errors.empty() && mErrors.empty());
  }
};
}  // namespace koda