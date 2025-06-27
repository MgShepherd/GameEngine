#include "render.h"
#include "instance_private.h"
#include "render_management.h"
#include "result.h"
#include "result_utils.h"
#include "sprite_private.h"
#include "vk_utils.h"

enum M_Result m_render_begin(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  if (instance->renderer.currently_rendering) {
    result = m_result_process(M_RENDER_ORDER_ERR, "Attempted to start render before ending previous render");
    return result;
  }
  instance->renderer.currently_rendering = true;

  return_result_if_err(m_renderer_render_begin(instance));

  return result;
}

enum M_Result m_render_end(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  if (!instance->renderer.currently_rendering) {
    result = m_result_process(M_RENDER_ORDER_ERR, "Attempted to end render before started");
    return result;
  }

  return_result_if_err(m_renderer_render_end(instance));
  instance->renderer.currently_rendering = false;

  return result;
}

enum M_Result m_render_sprite(const M_Instance *instance, const M_Sprite *sprite) {
  enum M_Result result = M_SUCCESS;
  if (!instance->renderer.currently_rendering) {
    result = m_result_process(M_RENDER_ORDER_ERR, "Attempted to render Sprite before render begin statement");
    return result;
  }

  m_renderer_render_buffer(instance, sprite->object.vertex_buf.vk_buffer, sprite->object.index_buf.vk_buffer,
                           sprite->object.index_buf.num_elements);

  return result;
}

enum M_Result m_render_sprites(const M_Instance *instance, M_Sprite **sprites, uint32_t num_sprites) {
  enum M_Result result = M_SUCCESS;

  for (uint32_t i = 0; i < num_sprites; i++) {
    return_result_if_err(m_render_sprite(instance, sprites[i]));
  }

  return result;
}
