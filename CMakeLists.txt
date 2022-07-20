cmake_minimum_required(VERSION 3.0.2)

project(patch
    VERSION 1.0.0
    LANGUAGES C
)

option(EXAMPLE_PATCHER "build patcher exe" OFF)

# export compile commands
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# enable testing
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)

    if (BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

add_subdirectory(src)

if (EXAMPLE_PATCHER)
    add_subdirectory(examples/patcher)
endif()
