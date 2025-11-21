# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1 < --linux | --windows >"
  echo "    --linux, linux      | Build the linux project"
  echo "    --windows, windows  | Build the windows project"
}

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
      --linux|linux)
      TARGET="linux"
      BUILD_PATH="build/linux"
      PREFIX_PATH="$HOME/Qt6-Linux"
      INSTALL_PREFIX="$HOME/dist/Linux"
      TOOLCHAIN_FILE=""
      shift
      ;;
      --windows|windows)
      TARGET="windows"
      BUILD_PATH="build/windows"
      PREFIX_PATH="$HOME/Qt6-Windows"
      INSTALL_PREFIX="$HOME/dist/Windows"
      TOOLCHAIN_FILE="cmake/toolchain-mingw64.cmake"
      shift
      ;;
      *)
      printHelp $0
      exit 1
      ;;
  esac
done

if [[ -z "${PREFIX_PATH}" ]]; then
  echo "Error: --linux or --windows required"
  printHelp "$0"
  exit 1
fi

# ----------------------------------
# Main
cmake -S . -B "$BUILD_PATH"\
  -DDEPLOY_TARGET="$TARGET" \
  -DCMAKE_PREFIX_PATH="$PREFIX_PATH" \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE"

cmake --build "$BUILD_PATH" -j 4