cmake_minimum_required(VERSION 3.0.2)

project(test_framework LANGUAGES C)
add_library(test_framework test_framework.c)
target_link_libraries(test_framework PRIVATE patch)
target_include_directories(test_framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ips)
add_subdirectory(ups)
add_subdirectory(bps)
