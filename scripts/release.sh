# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1 < --linux | --windows > --verbose"
  echo "    --linux, linux      | Build the linux project"
  echo "    --windows, windows  | Build the windows project"
  echo "    --verbose           | Show the tool logs"
}

LINUX="1"
export CROSSDEPLOYQT_VERBOSE=""

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
      --linux|linux)
      LINUX="1"
      shift
      ;;
      --windows|windows)
      LINUX="0"
      shift
      ;;
      --verbose)
      export CROSSDEPLOYQT_VERBOSE="true"
      shift
      ;;
      *)
      printHelp $0
      exit 1
      ;;
  esac
done

# ----------------------------------
# Constants
SRC_DIR=$HOME/maki
SCRIPT_DIR=$SRC_DIR/scripts
TOOL_DIR=$SCRIPT_DIR/crossdeployqt
TOOL_BUILD_DIR=$TOOL_DIR/build
INSTALL_DIR=$SRC_DIR/release/windows
WINDOWS_BUILD_DIR=$SRC_DIR/build/windows

# ----------------------------------
# Main
if [ "$LINUX" == "1" ]; then
  cmake --build build/linux -j 4 --target deploy-linux
else
  # Build the tool
  cd $TOOL_DIR
  mkdir -p build

  cd $TOOL_BUILD_DIR
  cmake ..
  make -j4

  # Make sure the output folder exists
  mkdir -p $INSTALL_DIR

  # Run the crossdeploy tool
  ./crossdeployqt --bin $WINDOWS_BUILD_DIR/maki.exe --out $INSTALL_DIR --no-qml

  # Copy extra libraries, this might not be needed outside wine
  cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll              $INSTALL_DIR
  cp /usr/lib/gcc/x86_64-w64-mingw32/13-win32/libgcc_s_seh-1.dll  $INSTALL_DIR
  cp /usr/lib/gcc/x86_64-w64-mingw32/13-win32/libstdc++-6.dll     $INSTALL_DIR

  # Copy assets and pluggins
  cp -r $WINDOWS_BUILD_DIR/share $INSTALL_DIR/share
  cp $WINDOWS_BUILD_DIR/plugins/* $INSTALL_DIR/plugins/
fi