cmake_minimum_required(VERSION 3.19)
project(gdextension)

include(FetchContent)

FetchContent_Declare(
        GDExtension
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.1.2-stable
)

FetchContent_MakeAvailable(GDExtension)

# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX src FILES ${SOURCES})

find_package(octopus)
find_package(flecs)

# Add a dynamic library called godoctopus - this builds godoctopus.dll
add_library(entity_drawer SHARED ${SOURCES})
target_link_libraries(entity_drawer PRIVATE godot::cpp octopus flecs::flecs_static)
target_include_directories(entity_drawer PRIVATE src)

install (TARGETS entity_drawer EXPORT ${PROJECT_NAME}-export)
