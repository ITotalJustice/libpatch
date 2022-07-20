cmake_minimum_required(VERSION 3.0.2)

project(ips LANGUAGES C)
add_library(ips ips.c)
target_include_directories(ips PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(ips PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
    >
)
