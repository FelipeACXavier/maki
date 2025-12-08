# ----------------------------------
# Main
cmake --build build/linux -j 4 --target deploy-linux

if [ $? -eq 0 ]; then
  echo "Creating compressed release file"
  tar czf ./release/maki-linux.tar.gz -C release/linux .
else
  echo "CMake failed, not creating compressed archive"
fi
