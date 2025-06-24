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
  const uint32_t num_sprites = 2;
  M_Sprite **sprites = NULL;
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

  sprites = malloc(sizeof(M_Sprite *) * num_sprites);
  if (sprites == NULL) {
    goto cleanup;
  }

  for (uint32_t i = 0; i < num_sprites; i++) {
    const struct M_SpriteProperties properties = {
        .x = 32.0f + (32.0f * i),
        .y = 32.0f + (32.0f * i),
        .width = 32.0f,
        .height = 32.0f,
        .color = {.r = 1.0f},
    };
    result = m_sprite_create(&sprites[i], instance, &properties);
  }
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  while (m_window_is_open(window)) {
    result = m_instance_update(instance, sprites, num_sprites);
    if (result != M_SUCCESS) {
      goto cleanup;
    }
    m_window_update(window);
  }

cleanup:
  for (uint32_t i = 0; i < num_sprites; i++) {
    m_sprite_destroy(sprites[i], instance);
    sprites[i] = NULL;
  }
  free(sprites);
  sprites = NULL;
  m_instance_destroy(instance);
  instance = NULL;
  m_window_destroy(window);
  window = NULL;

  return result == M_SUCCESS ? 0 : -1;
}
