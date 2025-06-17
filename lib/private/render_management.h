#ifndef RENDER_MANAGEMENT_H
#define RENDER_MANAGEMENT_H

#include "instance.h"
#include <vulkan/vulkan_core.h>

struct M_Renderer {
  VkCommandPool command_pool;
  VkCommandBuffer command_buffer;
  VkSemaphore image_available;
  VkSemaphore render_finished;
  VkFence in_flight;
};

enum M_Result m_renderer_create(struct M_Instance *instance, const VkPhysicalDevice physical_device);
void m_renderer_destroy(struct M_Instance *instance);

enum M_Result m_renderer_render(struct M_Instance *instance);

#endif // !RENDER_MANAGEMENT_H
