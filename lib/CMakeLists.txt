set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
	src/logger.c
	src/instance.c
	src/result.c
	src/vk_instance_helper.c
	src/vk_debug_messenger_helper.c
	src/vk_utils.c
	src/vk_device_management.c
	src/vk_swap_chain_management.c
	src/shader_management.c
	src/pipeline_management.c
	src/render_management.c
	src/buffer_management.c
	src/window.c)

find_package(Vulkan REQUIRED)

add_subdirectory(external/glfw)
add_subdirectory(external/cglm)

add_library(MEngine STATIC ${SOURCES})

target_include_directories(MEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(MEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)

target_link_libraries(MEngine glfw)
target_link_libraries(MEngine cglm)
target_link_libraries(MEngine Vulkan::Vulkan)
