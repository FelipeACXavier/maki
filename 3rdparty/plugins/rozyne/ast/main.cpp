#include <memory>

#include "KodaLexer.h"
#include "KodaParser.h"
#include "antlr4-runtime.h"
#include "ast.h"
#include "cst2ast.h"

int main(int argc, const char* argv[])
{
  koda::gPrintSpan = false;

  std::ifstream stream;
  stream.open(argv[1]);
  antlr4::ANTLRInputStream input(stream);
  KodaLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  KodaParser parser(&tokens);

  KodaParser::SystemContext* tree = parser.system();
  KodaCST2AST visitor;
  auto system = std::any_cast<koda::System>(visitor.visitSystem(tree));

  system.print();
  return 0;
}