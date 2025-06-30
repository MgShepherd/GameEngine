#include "input.h"
#include "instance.h"
#include "logger.h"
#include "render.h"
#include "result.h"
#include "sprite.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  M_Window *window = NULL;
  M_Instance *instance = NULL;
  const uint32_t num_sprites = 2;
  const char *app_name = "Game Engine";

  enum M_Result result = m_window_create(&window, app_name, 640, 480);
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  const M_InstanceOptions options = {
      .app_name = app_name,
      .enable_debug = true,
      .clear_color = {.r = 91.0f / 255.0f, .g = 188.f / 255.0f, .b = 228.0f / 255.0f},
  };
  result = m_instance_create(&instance, window, &options);
  if (result != M_SUCCESS) {
    goto cleanup;
  }
  const struct M_SpriteProperties paddle_1_properties = {
      .x = 32.0f,
      .y = 32.0f,
      .width = 20.0f,
      .height = 150.0f,
      .color = {.r = 1.0f},
  };
  const struct M_SpriteProperties paddle_2_properties = {
      .x = 640.0f - 32.0f - 20.0f,
      .y = 32.0f,
      .width = 20.0f,
      .height = 150.0f,
      .color = {.r = 1.0f},
  };

  M_Sprite *paddle_1, *paddle_2;
  result = m_sprite_create(&paddle_1, instance, &paddle_1_properties);
  if (result != M_SUCCESS) {
    goto cleanup;
  }
  result = m_sprite_create(&paddle_2, instance, &paddle_2_properties);
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  while (m_window_is_open(window)) {
    m_window_update(window);

    if (m_key_is_pressed(M_KEY_A)) {
      m_logger_info("Pressed the A key");
    }

    result = m_render_begin(instance);
    if (result != M_SUCCESS) {
      goto cleanup;
    }

    result = m_render_sprite(instance, paddle_1);
    if (result != M_SUCCESS) {
      goto cleanup;
    }
    result = m_render_sprite(instance, paddle_2);
    if (result != M_SUCCESS) {
      goto cleanup;
    }

    result = m_render_end(instance);
    if (result != M_SUCCESS) {
      goto cleanup;
    }
  }

cleanup:
  m_sprite_destroy(paddle_1, instance);
  paddle_1 = NULL;
  m_sprite_destroy(paddle_2, instance);
  paddle_2 = NULL;
  m_instance_destroy(instance);
  instance = NULL;
  m_window_destroy(window);
  window = NULL;

  return result == M_SUCCESS ? 0 : -1;
}
