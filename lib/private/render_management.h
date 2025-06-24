#ifndef RENDER_MANAGEMENT_H
#define RENDER_MANAGEMENT_H

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
};

enum M_Result m_renderer_create(struct M_Instance *instance);
void m_renderer_destroy(struct M_Instance *instance);

// TODO: Replace with render begin and end functions so that we can have functions which take in the objects to render
// individually Will allow for more customization from programs that are using the library
enum M_Result m_renderer_render(struct M_Instance *instance, struct M_Sprite **sprites, uint32_t num_sprites);

#endif // !RENDER_MANAGEMENT_H
