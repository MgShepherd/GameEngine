#ifndef RENDER_MANAGEMENT_H
#define RENDER_MANAGEMENT_H

#include "instance.h"
#include <vulkan/vulkan_core.h>

struct M_Renderer {
  VkCommandPool command_pool;
  VkCommandBuffer *command_buffers;
  VkSemaphore *image_available_semaphores;
  VkSemaphore *render_finished_semaphores;
  VkFence *in_flight_fences;
  uint32_t current_frame;
};

enum M_Result m_renderer_create(struct M_Instance *instance);
void m_renderer_destroy(struct M_Instance *instance);

enum M_Result m_renderer_render(struct M_Instance *instance);

#endif // !RENDER_MANAGEMENT_H
