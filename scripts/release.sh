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
  cmake --build build-linux -j 4 --target deploy-linux
else
  cmake --build build-windows -j 4 --target deploy-windows
fi