#include "window.h"
#include "logger.h"
#include "vulkan.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct M_Window {
  GLFWwindow *glfw_window;
  int width;
  int height;
};

struct M_Window *m_window_create(const char *title, int width, int height) {
  m_logger_init(M_INFO);

  if (glfwInit() == GLFW_FALSE) {
    return NULL;
  }

  struct M_Window *window = malloc(sizeof(struct M_Window));
  if (window == NULL) {
    return NULL;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  window->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
  window->width = width;
  window->height = height;
  if (window->glfw_window == NULL) {
    free(window);
    return NULL;
  }

  m_logger_info("Successfully initialised M_Window");

  VkInstance instance;
  m_vulkan_init(&instance, title);
  // TODO: Handle vulkan instance being null

  return window;
}

bool m_window_is_open(const struct M_Window *window) {
  assert(window != NULL);
  return !glfwWindowShouldClose(window->glfw_window);
}

void m_window_update(struct M_Window *window) {
  assert(window != NULL);
  glfwPollEvents();
}

void m_window_destroy(struct M_Window *window) {
  if (window != NULL && window->glfw_window != NULL) {
    glfwDestroyWindow(window->glfw_window);
    free(window);
  }
  glfwTerminate();
}
