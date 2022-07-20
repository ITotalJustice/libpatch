cmake_minimum_required(VERSION 3.0.2)

project(test_bps LANGUAGES C)
add_executable(test_bps test_bps.c)

target_link_libraries(test_bps PRIVATE
    patch
    test_framework
)

target_compile_options(test_bps PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
    >
)

add_test(
    NAME test_bps
    COMMAND test_bps
)
