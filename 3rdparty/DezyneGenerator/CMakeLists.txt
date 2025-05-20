cmake_minimum_required(VERSION 3.16)

set(PLUGIN_NAME json_plugin)
project(${PLUGIN_NAME})

message("Plugin files: ${PLUGIN_NAME}")

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_NAME}.cpp)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)

set_target_properties(${PLUGIN_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
