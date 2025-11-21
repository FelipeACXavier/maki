# cmake/toolchain-mingw64.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10)

# Target 64-bit Windows with MinGW-w64
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

# Optional: where MinGW lives; often not necessary on Ubuntu
# set(MINGW_ROOT "/usr/x86_64-w64-mingw32")
set(QT_HOST_PATH "/home/ubuntu/Qt6-Linux" CACHE PATH "host path for Qt")
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")

# Tell CMake how to search for libraries and headers
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
