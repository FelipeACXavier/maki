# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1 < --linux | --windows >"
  echo "    --linux, linux      | Build the linux project"
  echo "    --windows, windows  | Build the windows project"
}

LINUX="1"

while [[ $# -gt 0 ]]; do
  key="$1"
  echo "Arg: $key"
  case $key in
      --linux|linux)
      LINUX="1"
      shift
      ;;
      --windows|windows)
      LINUX="0"
      shift
      ;;
      *)
      printHelp $0
      exit 1
      ;;
  esac
done

# ----------------------------------
# Main
if [ "$LINUX" == "1" ]; then
  cmake --build build/linux -j 4 --target deploy-linux
else
  # cmake --build build/windows -j 4 --target deploy-windows
  # Make sure it is built
  cmake --build build/windows -j 4

  # Pack necessary files for windeployqt
  mkdir -p dist/windows-sdk

  # Copy Windows build artifacts
  cp -r build/windows dist/windows-sdk/build-windows

  # Copy cross-built Windows Qt prefix
  cp -r $HOME/Qt6-Windows dist/windows-sdk/Qt6-Windows

  # Copy qttools install which contains windeployqt.exe
  cp -r $HOME/qttools-install dist/windows-sdk/qttools-install

  zip -r dist/windows-sdk.zip dist/windows-sdk
fi