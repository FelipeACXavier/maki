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
  cmake -S . -B build/linux \
    -DDEPLOY_TARGET=linux \
    -DCMAKE_PREFIX_PATH=$HOME/Qt6-Linux \
    -DCMAKE_INSTALL_PREFIX=/home/ubuntu/MAKI/dist/linux

  cmake --build build/linux -j 4
else
  cmake -S . -B build/windows \
    -DDEPLOY_TARGET=windows \
    -DCMAKE_PREFIX_PATH=$HOME/Qt6-Windows \
    -DCMAKE_INSTALL_PREFIX=/home/ubuntu/MAKI/dist/windows \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw64.cmake

  cmake --build build/windows -j 4
fi