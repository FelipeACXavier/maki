cmake_minimum_required(VERSION 3.16)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPPHELPERS_FILE_SYSTEM OFF)
set(CPPHELPERS_USE_SOURCE ON)
set(CPPHELPERS_USE_QT ON)

function(suppress_warnings TARGET_NAME)
  if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W0)
  else()
    target_compile_options(${TARGET_NAME} PRIVATE -w)
  endif()
endfunction()

function(suppress_directory_warnings DIR)
  if(MSVC)
    set_property(DIRECTORY "${DIR}" APPEND PROPERTY COMPILE_OPTIONS /W0)
  else()
    set_property(DIRECTORY "${DIR}" APPEND PROPERTY COMPILE_OPTIONS -w)
  endif()
endfunction()

# Libraries
add_subdirectory(CppHelpers)
add_subdirectory(maki-sdk)

add_subdirectory(qclementine)
suppress_directory_warnings(qclementine)

block(SCOPE_FOR VARIABLES)
  if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
  else()
    add_compile_options(-w)
  endif()

  find_package(ZLIB REQUIRED)
  include_directories(${ZLIB_INCLUDE_DIRS})
  add_subdirectory(quazip)
endblock()

# ===================================
set(LIBAVOID_DIR ${CMAKE_CURRENT_SOURCE_DIR}/adaptagrams/cola/libavoid)
file(GLOB LIBAVOID_SOURCES
  ${LIBAVOID_DIR}/*.cpp
)
add_library(libavoid STATIC
  ${LIBAVOID_SOURCES}
)
if(MSVC)
  target_compile_definitions(libavoid PUBLIC
    NDEBUG
    LIBAVOID_NO_DLL
  )
endif()
target_include_directories(libavoid
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/adaptagrams/cola
)
suppress_warnings(libavoid)
# ===================================

# Build-in plugins
if (BUILD_PLUGINS)
  add_subdirectory(plugins/koda)
  add_subdirectory(plugins/platformio)
  add_subdirectory(plugins/arduino)
  add_subdirectory(plugins/ollama)
endif()
