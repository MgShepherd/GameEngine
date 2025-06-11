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

  result = m_instance_create(&instance, app_name);
  if (result != M_SUCCESS) {
    goto cleanup;
  }

  while (m_window_is_open(window)) {
    m_window_update(window);
  }

cleanup:
  m_instance_destroy(instance);
  m_window_destroy(window);

  return result == M_SUCCESS ? 0 : -1;
}
