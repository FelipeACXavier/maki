#include <filesystem>
#include <memory>

#include "koda_compiler.h"
#include "logging.h"
#include "result.h"

bool fileExists(const std::string& path)
{
  return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool parseCommandLine(int argc, const char* argv[], koda::CompilerOptions& options, std::string& error)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help")
    {
      options.showHelp = true;
      return true;
    }
    else if (arg == "--version")
    {
      options.showVersion = true;
      return true;
    }
    else if (arg == "-v")
      options.verbose = 1;
    else if (arg == "-vv")
      options.verbose = 2;
    else if (arg == "-vvv")
      options.verbose = 3;
    else if (arg == "--dry")
      options.dryRun = true;
    else if (arg == "-i" || arg == "--input")
    {
      if (i + 1 >= argc)
      {
        error = "Missing value for " + arg;
        return false;
      }
      options.inputFile = argv[++i];
    }
    else if (arg == "-o" || arg == "--output")
    {
      if (i + 1 >= argc)
      {
        error = "Missing value for " + arg;
        return false;
      }
      options.outputDir = argv[++i];
    }
    else
    {
      error = "Unknown argument: " + arg;
      return false;
    }
  }

  if (options.inputFile.empty())
  {
    error = "No input file provided. Use -i <file> or --input <file>.";
    return false;
  }

  if (!fileExists(options.inputFile))
  {
    error = "Input file does not exist: " + options.inputFile;
    return false;
  }

  return true;
}

void printHelp(const char* programName)
{
  std::cout
      << "Usage: " << programName << " [options]\n\n"
      << "Options:\n"
      << "  -i, --input <file>     Input Koda file\n"
      << "  -o, --output <dir>     Output directory (default: ./out)\n"
      << "      --dry              Do not create any files, just print what would be done\n"
      << "  -h, --help             Show this help message\n"
      << "      --version          Show version information\n"
      << "  -v                     Enable verbose output (can repeat: -vv, -vvv)\n";
}

void printVersion()
{
  std::cout << "koda-compiler version 1.0.0\n";
}

int main(int argc, const char* argv[])
{
  koda::CompilerOptions options;
  std::string error;

  if (!parseCommandLine(argc, argv, options, error))
  {
    std::cerr << "Error: " << error << "\n\n";
    printHelp(argv[0]);
    return 1;
  }

  if (options.showHelp)
  {
    printHelp(argv[0]);
    return 0;
  }

  if (options.showVersion)
  {
    printVersion();
    return 0;
  }

  std::error_code ec;
  std::filesystem::create_directories(options.outputDir, ec);
  if (ec)
  {
    std::cerr << "Error: Failed to create output directory: "
              << options.outputDir << "\nReason: " << ec.message() << "\n";
    return 1;
  }

  koda::gPrintSpan = (options.verbose > 1);

  koda::Compiler compiler(options);
  auto parsed = compiler.parse(options.inputFile);
  LOG_ERROR_ON_FAILURE(parsed);
  if (!parsed.IsSuccess())
    return -1;

  auto generated = compiler.generate();
  LOG_ERROR_ON_FAILURE(generated);
  if (!generated.IsSuccess())
    return -1;

  return 0;
}