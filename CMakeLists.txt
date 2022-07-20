cmake_minimum_required(VERSION 3.0.2)

option(EXAMPLE_PATCHER "build patcher exe" OFF)


project(patch LANGUAGES C)
add_library(patch patch.c)

add_subdirectory(ips)
add_subdirectory(ups)
add_subdirectory(bps)

target_link_libraries(patch PRIVATE ips ups bps)
target_include_directories(patch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(patch PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
    >
    $<$<C_COMPILER_ID:MSVC>:
        /W4
    >
)

if (EXAMPLE_PATCHER)
    add_subdirectory(examples/patcher)
endif()
