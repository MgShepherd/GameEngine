#ifndef M_RENDER_MANAGEMENT_H
#define M_RENDER_MANAGEMENT_H

#include "instance.h"
#include "sprite.h"
#include <vulkan/vulkan_core.h>

#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 3
#endif

struct M_Renderer {
  VkCommandPool command_pool;
  VkCommandBuffer *command_buffers;
  VkSemaphore *image_available_semaphores;
  VkSemaphore *render_finished_semaphores;
  VkFence *in_flight_fences;
  uint32_t current_frame;
  uint32_t current_image_idx;
  bool currently_rendering;
};

enum M_Result m_renderer_create(struct M_Instance *instance);
void m_renderer_destroy(struct M_Instance *instance);

enum M_Result m_renderer_render_begin(struct M_Instance *instance);
enum M_Result m_renderer_render_end(struct M_Instance *instance);
enum M_Result m_renderer_render_buffer(const struct M_Instance *instance, VkBuffer vertex_buf, VkBuffer index_buf,
                                       uint32_t num_indices);

#endif
