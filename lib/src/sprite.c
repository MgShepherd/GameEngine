#include "sprite.h"
#include "instance_private.h"
#include "render_management.h"
#include "render_object.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "sprite_private.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_INDICES = 6;
const uint32_t NUM_VERTICES = 4;
const uint32_t indices[] = {0, 1, 2, 2, 3, 0};

enum M_Result m_sprite_create(struct M_Sprite **sprite, const M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  *sprite = malloc(sizeof(struct M_Sprite));
  return_result_if_null(sprite, M_MEMORY_ALLOC_ERR, "Unable to allocate memory for sprite");

  const struct M_Vertex vertices[] = {
      {.position = {-0.5f, -0.5f}, .color = {1.0f, 0.0f, 0.0f}},
      {.position = {0.5f, -0.5f}, .color = {0.0f, 1.0f, 0.0f}},
      {.position = {0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}},
      {.position = {-0.5f, 0.5f}, .color = {1.0f, 1.0f, 1.0f}},
  };

  m_render_object_create(&(*sprite)->object, instance, vertices, NUM_VERTICES, indices, NUM_INDICES);

  return result;
}

void m_sprite_destroy(struct M_Sprite *sprite, const M_Instance *instance) {
  vkDeviceWaitIdle(instance->device.vk_device);
  if (sprite != NULL) {
    m_render_object_destroy(&sprite->object, instance);
    free(sprite);
  }
}
