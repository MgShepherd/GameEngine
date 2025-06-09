cmake_minimum_required(VERSION 3.24)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
	src/main.c
	src/logger.c)

add_subdirectory(external/glfw)

project(GameEngine)

add_executable(GameEngine ${SOURCES})

target_include_directories(GameEngine PUBLIC ./include)
target_link_libraries(GameEngine glfw)
