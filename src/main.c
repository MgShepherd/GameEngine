#include "logger.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
  logger_init(LOG_LEVEL_INFO);

  if (glfwInit() == GLFW_FALSE) {
    printf("Unable to initialise GLFW\n");
    return -1;
  }

  GLFWwindow *window = glfwCreateWindow(640, 480, "Game Engine", NULL, NULL);
  if (window == NULL) {
    return -1;
  }
  logger_info("Successfully initialised GLFW and created window");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
