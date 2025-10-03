# plugins/cppgenerator/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
set(PLUGIN_NAME rozyne_generatorplugin)

project(${PLUGIN_NAME})
find_package(Qt6 REQUIRED COMPONENTS Core)

set(CMAKE_AUTOMOC ON)

add_library(${PLUGIN_NAME} SHARED
  rozyne_generator.cpp
  rozyne_generator.h
  rozyne_generator.json
)

target_link_libraries(${PLUGIN_NAME} PRIVATE 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    libcommon
    libcpphelpers)

target_include_directories(${PLUGIN_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/app
)

# Put the built .so/.dll in a plugins folder next to your app
set_target_properties(${PLUGIN_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
