#include "logger.h"
#include "window.h"

#include <stdio.h>

int main() {
  m_logger_init(M_INFO);

  M_Window *window = m_window_create("Game Engine", 640, 480);
  if (window == NULL) {
    return -1;
  }

  while (m_window_is_open(window)) {
    m_window_update(window);
  }

  m_window_destroy(window);

  return 0;
}
