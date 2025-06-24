#include "instance.h"
#include "logger.h"
#include "result.h"
#include "sprite.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  M_Window *window = NULL;
  M_Instance *instance = NULL;
  M_Sprite *sprite = NULL;
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

  const struct M_SpriteProperties properties = {
      .x = 32.0f,
      .y = 32.0f,
      .width = 32.0f,
      .height = 32.0f,
      .color = {.r = 1.0f},
  };
  result = m_sprite_create(&sprite, instance, &properties);
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  while (m_window_is_open(window)) {
    result = m_instance_update(instance, sprite);
    if (result != M_SUCCESS) {
      goto cleanup;
    }
    m_window_update(window);
  }

cleanup:
  m_sprite_destroy(sprite, instance);
  sprite = NULL;
  m_instance_destroy(instance);
  instance = NULL;
  m_window_destroy(window);
  window = NULL;

  return result == M_SUCCESS ? 0 : -1;
}
