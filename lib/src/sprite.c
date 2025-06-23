#include "sprite.h"
#include "cglm/types.h"
#include "instance_private.h"
#include "logger.h"
#include "render_management.h"
#include "render_object.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "sprite_private.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#define color_to_vec3(properties) {properties->color.r, properties->color.g, properties->color.b}

const uint32_t NUM_INDICES = 6;
const uint32_t NUM_VERTICES = 4;
const uint32_t indices[] = {0, 1, 2, 2, 3, 0};

bool validate_dimension(float dimension) { return dimension > 0 && dimension <= 2.0f; }

bool validate_location(float location) { return location >= -1.0f && location <= 1.0f; }

bool validate_sprite_properties(const struct M_SpriteProperties *properties) {
  return validate_location(properties->x) && validate_location(properties->y) &&
         validate_dimension(properties->width) && validate_dimension(properties->height);
}

enum M_Result m_sprite_create(M_Sprite **sprite, const M_Instance *instance,
                              const struct M_SpriteProperties *properties) {
  enum M_Result result = M_SUCCESS;

  *sprite = malloc(sizeof(struct M_Sprite));
  return_result_if_null(sprite, M_MEMORY_ALLOC_ERR, "Unable to allocate memory for sprite");

  if (!validate_sprite_properties(properties)) {
    result = m_result_process(M_INPUT_ERR, "Invalid properties provided for Sprite creation");
    return result;
  }

  const struct M_Vertex vertices[] = {
      {.position = {properties->x, properties->y}, .color = color_to_vec3(properties)},
      {.position = {properties->x + properties->width, properties->y}, .color = color_to_vec3(properties)},
      {.position = {properties->x + properties->width, properties->y + properties->height},
       .color = color_to_vec3(properties)},
      {.position = {properties->x, properties->y + properties->height}, .color = color_to_vec3(properties)},
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
