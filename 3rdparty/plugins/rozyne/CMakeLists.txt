# plugins/cppgenerator/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
set(PLUGIN_NAME rozyne_generatorplugin)

project(${PLUGIN_NAME})
find_package(Qt6 REQUIRED COMPONENTS Core)

set(CMAKE_AUTOMOC ON)

# ==================================================================
# Plugin generator
find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Widgets
  WebSockets
)

add_library(${PLUGIN_NAME} SHARED
  dzn_client/dezyne_client.cpp
  dzn_client/dezyne_simulator.cpp
  dzn_client/simulation_scene.cpp
  dzn_client/simulation_label_item.cpp
  dzn_client/component_model.cpp
  rozyne_generator.cpp
  rozyne_generator.h
  rozyne_generator.json
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebSockets
    libcommon
    libcpphelpers
    libmaki)

target_include_directories(${PLUGIN_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${CMAKE_CURRENT_SOURCE_DIR}/dzn_client
)

set_target_properties(${PLUGIN_NAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_FOLDER}
  RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_FOLDER}
)

# ==================================================================
# Koda language
set(COMPILER_NAME "kodagen")
add_executable(${COMPILER_NAME}
  ast/main.cpp
  ast/ast.cpp
  ast/cst2ast.cpp
  parser/KodaParser.cpp
  parser/KodaLexer.cpp
  parser/KodaVisitor.cpp
  parser/KodaBaseVisitor.cpp
)

find_path(ANTLR4_RUNTIME_INCLUDE_DIR
  NAMES antlr4-runtime.h
  PATH_SUFFIXES antlr4-runtime
  PATHS /usr/include /usr/local/include
)

find_library(ANTLR4_RUNTIME_LIBRARY
  NAMES antlr4-runtime
  PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
)

target_link_libraries(${COMPILER_NAME} PRIVATE
  ${ANTLR4_RUNTIME_LIBRARY}
  libcommon
  libcpphelpers
)

target_include_directories(${COMPILER_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/parser
  ${CMAKE_CURRENT_SOURCE_DIR}/ast
  ${ANTLR4_RUNTIME_INCLUDE_DIR}
)

# ==================================================================
# Dezyne client
find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Widgets
  WebSockets
)

set(DZN_CLIENT_NAME "dzn_client")
add_executable(${DZN_CLIENT_NAME}
  dzn_client/main.cpp
  dzn_client/dezyne_client.cpp
  dzn_client/dezyne_simulator.cpp
  # dzn_client/simulation_scene.cpp
)

target_link_libraries(${DZN_CLIENT_NAME} PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::WebSockets
  libcommon
  libcpphelpers
)

target_include_directories(${DZN_CLIENT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/dzn_client
)