#include "result.h"
#include "window.h"

#include <stdio.h>

int main() {
  M_Window *window;
  const enum M_Result result = m_window_create(&window, "Game Engine", 640, 480);
  if (result != M_SUCCESS) {
    return -1;
  }

  while (m_window_is_open(window)) {
    m_window_update(window);
  }

  m_window_destroy(window);

  return 0;
}
