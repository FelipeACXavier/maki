# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1 < --linux | --windows >"
  echo "    --linux, linux      | Build the linux project"
  echo "    --windows, windows  | Build the windows project"
  echo "    --local-qt          | Path to the local Qt installation"
  echo "    --local-project     | Path to the local project"
  echo "    --clean             | Clean the build directory"
  echo ""
}

CLEAN=0
SOURCE_DIR=$HOME/maki

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
      --linux|linux)
      TARGET="linux"
      BUILD_PATH="$SOURCE_DIR/build/linux"
      PREFIX_PATH="$HOME/Qt6-Linux"
      INSTALL_PREFIX="$SOURCE_DIR/release/linux"
      TOOLCHAIN_FILE=""
      shift
      ;;
      --windows|windows)
      TARGET="windows"
      BUILD_PATH="$SOURCE_DIR/build/windows"
      PREFIX_PATH="$HOME/Qt6-Windows"
      INSTALL_PREFIX="$SOURCE_DIR/release/windows"
      TOOLCHAIN_FILE="cmake/toolchain-mingw64.cmake"
      shift
      ;;
      --local-qt)
      LOCAL_QT_PATH="$2"
      shift
      shift
      ;;
      --local-project)
      LOCAL_PROJECT_PATH="$2"
      shift
      shift
      ;;
      --clean)
      CLEAN=1
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

echo "Running with:"
echo "  TARGET: ${TARGET}"
echo "  BUILD_PATH: ${BUILD_PATH}"
echo "  PREFIX_PATH: ${PREFIX_PATH}"
echo "  INSTALL_PREFIX: ${INSTALL_PREFIX}"
echo "  TOOLCHAIN_FILE: ${TOOLCHAIN_FILE}"

# ----------------------------------
# Main
if [ $CLEAN -eq 1 ]; then
  echo "Cleaning $BUILD_PATH"
  rm -rf $BUILD_PATH
else
  cmake -S . -B "$BUILD_PATH"\
    -DDEPLOY_TARGET="$TARGET" \
    -DCMAKE_PREFIX_PATH="$PREFIX_PATH" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DLOCAL_QT_PATH="$LOCAL_QT_PATH" \
    -DLOCAL_PROJECT_PATH="$LOCAL_PROJECT_PATH"

  cmake --build "$BUILD_PATH" -j 4
fi