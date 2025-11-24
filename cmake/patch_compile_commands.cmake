# cmake/patch_compile_commands.cmake
file(READ "${INPUT}" _contents)
string(REPLACE "${OLD}" "${NEW}" _patched "${_contents}")
file(WRITE "${INPUT}" "${_patched}")
