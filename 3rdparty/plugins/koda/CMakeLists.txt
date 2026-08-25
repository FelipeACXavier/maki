# plugins/cppgenerator/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
set(PLUGIN_NAME koda_generatorplugin)

project(${PLUGIN_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# If we are building with MAKI (with BUILD_PLUGINS), then just use MAKI's settings
if (NOT BUILD_PLUGINS)
  if (MSVC)
    set(antlr4-runtime_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/share/antlr4-runtime")
    add_compile_options(/W4 /MP)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_compile_options(/Zi /Od)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
      add_compile_options(/O2)
    endif()

    find_package(Qt6 ${QT_VERSION} REQUIRED COMPONENTS
      ${QT_REQUIRED_PACKAGES}
    )
  else()
    add_compile_options(-Wall)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      message(STATUS "Compiling with debug symbols")
      set(LINUX_BUILD_DIR "${CMAKE_SOURCE_DIR}/build/linux/debug")
      add_compile_options(-g -O0)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
      message("Compiling in release version")
      set(LINUX_BUILD_DIR "${CMAKE_SOURCE_DIR}/build/linux/release")
      add_compile_options(-O2)
    endif()

    find_package(Qt6 REQUIRED COMPONENTS
      ${QT_REQUIRED_PACKAGES}
    )
  endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebSockets)
find_package(antlr4-runtime CONFIG REQUIRED)

# Sources
set(KODA_DZN_CLIENT_FILES
  src/dzn_client/dezyne_client.cpp
  src/dzn_client/dezyne_simulator.cpp
  src/dzn_client/simulation_scene.cpp
  src/dzn_client/simulation_label_item.cpp
  src/dzn_client/component_model.cpp
)

set(KODA_COMPILER_FILES
  src/ast/ast.cpp
  src/ast/cst2ast.cpp

  src/ast/declaration_pass.cpp
  src/ast/ir_builder.cpp
  src/ast/koda_compiler.cpp

  src/ast/main.cpp
  src/ast/semantic_analyzer.cpp
  src/ast/symbol_registry.cpp

  src/ast/emitters/dezyne_declaration_pass.cpp
  src/ast/emitters/dezyne_emitter.cpp
  src/ast/emitters/dezyne_lowering_pass.cpp
  src/ast/emitters/dezyne_model.cpp
  src/ast/emitters/dezyne_writer.cpp
  src/ast/emitters/dezyne_library_emitter.cpp
  src/ast/emitters/dezyne_ros_emitter.cpp

  src/parser/KodaParser.cpp
  src/parser/KodaLexer.cpp
  src/parser/KodaVisitor.cpp
  src/parser/KodaBaseVisitor.cpp

  src/traceability_map.cpp
)
# ==================================================================
# Koda language
set(COMPILER_NAME "kodagen")
add_executable(${COMPILER_NAME}
  src/ast/main.cpp
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
  ${ANTLR4_RUNTIME_LIBRARY}
  libcpphelpers
  libmaki
)

target_include_directories(${COMPILER_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_include_directories(${COMPILER_NAME} SYSTEM PRIVATE
  ${ANTLR4_RUNTIME_INCLUDE_DIR}
)

# ==================================================================
# Koda library
set(LIB_NAME "libkoda")
add_library(${LIB_NAME} STATIC ${KODA_COMPILER_FILES})

target_include_directories(${LIB_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(${LIB_NAME}
  SYSTEM PUBLIC
    ${ANTLR4_RUNTIME_INCLUDE_DIR}
)

target_link_libraries(${LIB_NAME} PRIVATE
  ${ANTLR4_RUNTIME_LIBRARY}
  libcpphelpers
  libmaki
)

install(TARGETS ${LIB_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ==================================================================
# Plugin generator
find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Svg
  Widgets
  WebSockets
)

# set(DEZYNE_LIB_DIR "${CMAKE_BINARY_DIR}/share/plugins/KODA_ANTLR/lib")
file(GLOB DEZYNE_LIB_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/lib/*.dzn
)

function(add_koda_plugin TARGET_NAME USE_ANTLR_ENABLED)
  # if(USE_ANTLR_ENABLED)
    set(JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/koda_generator.json)
    set(PLUGIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/koda_plugin.json)
    set(COMPILER_FILES
      ${KODA_COMPILER_FILES}
    )
    set(KODA_TO_MAKI
      src/maki_to_koda.cpp
      src/koda_emitter.cpp
      # src/ros_emitter.cpp
    )
    set(COMPILER_INCLUDES
      ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    set(ANTLR_INCLUDES
      ${ANTLR4_RUNTIME_INCLUDE_DIR}
    )
    set(COMPILER_LIBS
      ${ANTLR4_RUNTIME_LIBRARY}
    )

  add_library(${TARGET_NAME} SHARED
    ${KODA_DZN_CLIENT_FILES}
    ${COMPILER_FILES}
    src/koda_generator.cpp
    src/koda_generator.h
    src/svg_tool_button.cpp

    ${KODA_TO_MAKI}
    src/actions/koda_action.cpp
    src/actions/dezyne_action.cpp
    src/actions/cpp_action.cpp
    src/actions/verify_action.cpp
    src/actions/simulate_action.cpp
    src/actions/ros_action.cpp
    src/actions/ros_copy.cpp
    src/actions/ros_build.cpp
    src/actions/ros_launch.cpp
    ${JSON_FILE}

    assets/assets.qrc
  )

  target_link_libraries(${TARGET_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebSockets
    Qt6::Svg
    ${COMPILER_LIBS}
    libcpphelpers
    libmaki
  )

  target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dzn_client
    ${COMPILER_INCLUDES}
  )
  target_include_directories(${TARGET_NAME} SYSTEM PRIVATE
    ${ANTLR_INCLUDES}
  )

  # if(USE_ANTLR_ENABLED)
    target_compile_definitions(${TARGET_NAME} PRIVATE USE_ANTLR=1)
  # endif()

  # Make sure the antlr libraries are available in the target
  if (WIN32)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${COMPILER_LIBS}>
        $<TARGET_FILE_DIR:${TARGET_NAME}>
      COMMAND_EXPAND_LISTS
    )
  endif()

  maki_stage_plugin(
    PLUGIN_ID ${TARGET_NAME}
    TARGET ${TARGET_NAME}
    METADATA ${PLUGIN_FILE}
    BIN_TARGETS
      ${COMPILER_NAME}
      ${TEST_NAME}
      ${DZN_CLIENT_NAME}
    ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets
  )
endfunction()

include(${CMAKE_SOURCE_DIR}/3rdparty/maki-sdk/cmake/maki-plugin.cmake)

# If we are building with MAKI, then use the MAKI sdk instead of the 3rdparty one
if (NOT BUILD_PLUGINS)
  add_subdirectory(3rdparty)
endif()

add_koda_plugin(koda_generatorplugin ON)
