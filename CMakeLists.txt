cmake_minimum_required(VERSION 3.24)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

project(GameEngine)

add_executable(GameEngine src/main.c)

target_link_libraries(GameEngine glfw)
