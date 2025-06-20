#include "instance.h"
#include "result.h"
#include "window.h"

#include <stdio.h>

int main() {
  M_Window *window = NULL;
  M_Instance *instance = NULL;
  const char *app_name = "Game Engine";

  enum M_Result result = m_window_create(&window, app_name, 640, 480);
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  const M_InstanceOptions options = {
      .app_name = app_name,
      .enable_debug = true,
  };
  result = m_instance_create(&instance, window, &options);
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  while (m_window_is_open(window)) {
    result = m_instance_update(instance);
    if (result != M_SUCCESS) {
      goto cleanup;
    }
    m_window_update(window);
  }

cleanup:
  m_instance_destroy(instance);
  instance = NULL;
  m_window_destroy(window);
  window = NULL;

  return result == M_SUCCESS ? 0 : -1;
}
