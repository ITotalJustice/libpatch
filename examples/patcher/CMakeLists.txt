cmake_minimum_required(VERSION 3.0.2)

project(patcher LANGUAGES C)
add_executable(patcher main.c)
target_link_libraries(patcher PRIVATE patch)

target_compile_options(patcher PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
    >
)

set_target_properties(patcher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
