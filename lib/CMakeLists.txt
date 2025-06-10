set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
	src/logger.c
	src/vulkan.c
	src/result.c
	src/window.c)

find_package(Vulkan REQUIRED)

add_subdirectory(external/glfw)

add_library(MEngine STATIC ${SOURCES})

target_include_directories(MEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(MEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)

target_link_libraries(MEngine glfw)
target_link_libraries(MEngine Vulkan::Vulkan)
