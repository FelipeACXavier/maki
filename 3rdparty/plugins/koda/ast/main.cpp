#include <memory>

#include "koda_compiler.h"
#include "logging.h"
#include "result.h"

int main(int argc, const char* argv[])
{
  koda::gPrintSpan = false;

  if (argc < 2)
  {
    LOG_ERROR("No input file provided");
    return -1;
  }

  koda::Compiler compiler;
  auto parsed = compiler.parse(argv[1]);
  LOG_ERROR_ON_FAILURE(parsed);
  if (!parsed.IsSuccess())
    return -1;

  compiler.printAST();

  auto generated = compiler.generate();
  LOG_ERROR_ON_FAILURE(generated);
  if (!generated.IsSuccess())
    return -1;

  return 0;
}