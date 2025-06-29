set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
	src/logger.c
	src/instance.c
	src/result.c
	src/render.c
	src/window.c
	src/sprite.c
	src/input.c
	src/private/vulkan_management.c
	src/private/debug_messenger.c
	src/private/m_utils.c
	src/private/device_management.c
	src/private/swap_chain_management.c
	src/private/shader_management.c
	src/private/pipeline_management.c
	src/private/render_management.c
	src/private/buffer_management.c
	src/private/render_object.c
	src/private/uniform_management.c)

find_package(Vulkan REQUIRED)

add_subdirectory(external/glfw)
add_subdirectory(external/cglm)

add_library(MEngine STATIC ${SOURCES})

target_include_directories(MEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(MEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)

target_link_libraries(MEngine glfw)
target_link_libraries(MEngine cglm)
target_link_libraries(MEngine Vulkan::Vulkan)
