cmake_minimum_required(VERSION 3.0.2)

project(bps LANGUAGES C)
add_library(bps bps.c)
target_include_directories(bps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(bps PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
    >
)
