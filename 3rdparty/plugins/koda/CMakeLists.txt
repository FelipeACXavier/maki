# plugins/cppgenerator/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
set(PLUGIN_NAME koda_generatorplugin)

project(${PLUGIN_NAME})
if (MSVC)
set(antlr4-runtime_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/share/antlr4-runtime")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebSockets)
find_package(antlr4-runtime CONFIG REQUIRED)

set(CMAKE_AUTOMOC ON)

# ==================================================================
# Koda language
set(KODA_COMPILER_FILES
  ast/ast.cpp
  ast/cst2ast.cpp
  ast/koda_compiler.cpp
  parser/KodaParser.cpp
  parser/KodaLexer.cpp
  parser/KodaVisitor.cpp
  parser/KodaBaseVisitor.cpp
)

set(COMPILER_NAME "kodagen")
add_executable(${COMPILER_NAME}
  ast/main.cpp
  ${KODA_COMPILER_FILES}
)

if (MSVC)
set(ANTLR4_RUNTIME_LIBRARY antlr4_shared)
else()
find_path(ANTLR4_RUNTIME_INCLUDE_DIR
  NAMES antlr4-runtime.h
  PATH_SUFFIXES antlr4-runtime
  PATHS /usr/include /usr/local/include
)

find_library(ANTLR4_RUNTIME_LIBRARY
  NAMES antlr4-runtime
  PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
)
endif()

target_link_libraries(${COMPILER_NAME} PRIVATE
  # ${ANTLR4_RUNTIME_LIBRARY}
  antlr4_shared
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
set(DZN_CLIENT_NAME "dzn_client")
add_executable(${DZN_CLIENT_NAME}
  dzn_client/main.cpp
  dzn_client/dezyne_client.cpp
  dzn_client/dezyne_simulator.cpp
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

# ==================================================================
# Plugin generator
find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Widgets
  WebSockets
)

set(DEZYNE_LIB_DIR "${CMAKE_BINARY_DIR}/share/plugins/KODA_ANTLR/lib")
file(GLOB DEZYNE_LIB_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.dzn
)

add_custom_target(copy_dezyne_libs ALL
  COMMENT "Copying Dezyne libs to ${DEZYNE_LIB_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DEZYNE_LIB_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DEZYNE_LIB_FILES} ${DEZYNE_LIB_DIR}
  DEPENDS ${DEZYNE_LIB_FILES}
)

function(add_koda_plugin TARGET_NAME USE_ANTLR_ENABLED)
  if(USE_ANTLR_ENABLED)
    set(JSON_FILE koda_generator_antlr.json)
    set(COMPILER_FILES ${KODA_COMPILER_FILES})
    set(COMPILER_INCLUDES
      ${CMAKE_CURRENT_SOURCE_DIR}/ast
      ${CMAKE_CURRENT_SOURCE_DIR}/parser
      ${ANTLR4_RUNTIME_INCLUDE_DIR}
    )
    set(COMPILER_LIBS
      ${ANTLR4_RUNTIME_LIBRARY}
    )
  else()
    set(JSON_FILE koda_generator.json)
    set(COMPILER_FILES)
    set(COMPILER_INCLUDES)
    set(COMPILER_LIBS)
  endif()

  add_library(${TARGET_NAME} SHARED
    dzn_client/dezyne_client.cpp
    dzn_client/dezyne_simulator.cpp
    dzn_client/simulation_scene.cpp
    dzn_client/simulation_label_item.cpp
    dzn_client/component_model.cpp
    ${COMPILER_FILES}
    koda_generator.cpp
    koda_generator.h
    ${JSON_FILE}
  )

  target_link_libraries(${TARGET_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebSockets
    ${COMPILER_LIBS}
    libcommon
    libcpphelpers
    libmaki
  )

  target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/dzn_client
    ${COMPILER_INCLUDES}
  )

  if(USE_ANTLR_ENABLED)
    target_compile_definitions(${TARGET_NAME} PRIVATE USE_ANTLR=1)
    add_dependencies(${TARGET_NAME} copy_dezyne_libs)
  endif()

  set_target_properties(${TARGET_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_FOLDER}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_FOLDER}
  )

  set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/plugins"
  )
endfunction()

add_koda_plugin(koda_generatorplugin OFF)
add_koda_plugin(koda_generatorplugin_antlr ON)
