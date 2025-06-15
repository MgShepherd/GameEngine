#ifndef WINDOW_PRIVATE_H
#define WINDOW_PRIVATE_H

#include "GLFW/glfw3.h"

struct M_Window {
  GLFWwindow *glfw_window;
  int width;
  int height;
};

#endif
