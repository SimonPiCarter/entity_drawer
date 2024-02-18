cmake_minimum_required(VERSION 3.19)


# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX src FILES ${SOURCES})

add_executable(grid ${SOURCES})
target_include_directories(grid PRIVATE src)
target_link_libraries(grid PRIVATE octopus)

install (TARGETS grid EXPORT ${PROJECT_NAME}-export)
