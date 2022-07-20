cmake_minimum_required(VERSION 3.0.2)

project(test_ips LANGUAGES C)
add_executable(test_ips test_ips.c)

target_link_libraries(test_ips PRIVATE
    patch
    test_framework
)

target_compile_options(test_ips PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
    >
)

add_test(
    NAME test_ips
    COMMAND test_ips
)
