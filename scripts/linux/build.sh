# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1"
  echo ""
  echo "    --release           | Create release"
  echo "    --local-qt          | Path to the local Qt installation"
  echo "    --local-project     | Path to the local project"
  echo "    --clean             | Clean the build directory"
  echo ""
}

CLEAN=0
BUILD_TYPE="Debug"
TARGET="linux"
CURR_DIR=`pwd`
SOURCE_DIR=$CURR_DIR
# Use QT version from the single source of truth file
QT_VERSION="$(tr -d ' \n' < $CURR_DIR/.qt-version)"
BUILD_PATH="$SOURCE_DIR/build/linux/debug"
PREFIX_PATH="$HOME/Qt/$QT_VERSION/gcc_64"
INSTALL_PREFIX="$SOURCE_DIR/release/linux"
EXTRA_ARGS=""

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
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
      --release)
      BUILD_TYPE="Release"
      BUILD_PATH="$SOURCE_DIR/build/linux/release"
      shift
      ;;
      --prefix)
      PREFIX_PATH="$2"
      shift
      shift
      ;;
      --help)
      printHelp $0
      exit 0
      ;;
      *)
      EXTRA_ARGS="${EXTRA_ARGS} $1"
      shift
      ;;
  esac
done

echo "--------------------------------------"
echo "Running with:"
echo "  CURR_DIR: ${CURR_DIR}"
echo "  QT_VERSION: ${QT_VERSION}"
echo "  BUILD_TYPE: ${BUILD_TYPE}"
echo "  TARGET: ${TARGET}"
echo "  BUILD_PATH: ${BUILD_PATH}"
echo "  PREFIX_PATH: ${PREFIX_PATH}"
echo "  INSTALL_PREFIX: ${INSTALL_PREFIX}"
echo "  Extra args: ${EXTRA_ARGS}"
echo "--------------------------------------"

# ----------------------------------
# Main
if [ $CLEAN -eq 1 ]; then
  echo "Cleaning $BUILD_PATH"
  rm -rf $BUILD_PATH
  rm -rf $INSTALL_PREFIX
else
  cmake -S "$SOURCE_DIR" -B "$BUILD_PATH"\
    -DDEPLOY_TARGET="$TARGET" \
    -DCMAKE_PREFIX_PATH="$PREFIX_PATH" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DLOCAL_QT_PATH="$LOCAL_QT_PATH" \
    -DLOCAL_PROJECT_PATH="$LOCAL_PROJECT_PATH" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    $EXTRA_ARGS

  if [ "$BUILD_TYPE" == "Release" ]; then
    cmake --build "$BUILD_PATH" -j 4 --target deploy-linux
  else
    cmake --build "$BUILD_PATH" -j 4
  fi
fi
