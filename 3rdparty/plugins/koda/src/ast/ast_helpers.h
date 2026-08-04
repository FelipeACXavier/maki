// AstHelpers.h
#pragma once
#include <string>

#include "antlr4-runtime.h"

inline koda::Span spanOf(antlr4::ParserRuleContext* ctx)
{
  koda::Span s;
  auto* a = ctx->getStart();
  auto* b = ctx->getStop();
  if (a)
  {
    s.lineStart = a->getLine();
    s.colStart = a->getCharPositionInLine();
  }
  if (b)
  {
    s.lineEnd = b->getLine();
    s.colEnd = b->getCharPositionInLine();
  }

  return s;
}

inline std::string unquoteString(const std::string& s)
{
  // minimal unquote; you can expand to handle escapes if needed.
  if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
    return s.substr(1, s.size() - 2);

  return s;
}
