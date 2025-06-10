#include "window.h"
#include "logger.h"
#include "result.h"
#include "vulkan.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

struct M_Window {
  GLFWwindow *glfw_window;
  int width;
  int height;
};

void glfw_error_callback(int error, const char *description) {
  m_result_process(M_WINDOW_INIT_ERR, description);
}

enum M_Result m_window_create(M_Window **window, const char *title, int width, int height) {
  enum M_Result result = M_SUCCESS;
  m_logger_init(M_INFO);

  glfwSetErrorCallback(glfw_error_callback);

  if (glfwInit() == GLFW_FALSE) {
    result = M_WINDOW_INIT_ERR;
    goto window_create_cleanup;
  }

  *window = malloc(sizeof(struct M_Window));
  if (window == NULL) {
    result = M_WINDOW_INIT_ERR;
    m_result_process(M_WINDOW_INIT_ERR, "Unable to allocate memory required for Window object");
    goto window_create_cleanup;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  (*window)->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
  (*window)->width = width;
  (*window)->height = height;

  if ((*window)->glfw_window == NULL) {
    result = M_WINDOW_INIT_ERR;
    goto window_create_cleanup;
  }

  m_logger_info("Successfully initialised M_Window");

  VkInstance instance = NULL;
  result = m_vulkan_init(&instance, title);

window_create_cleanup:
  if (result != M_SUCCESS) {
    if (*window != NULL)
      m_window_destroy(*window);
    if (instance != NULL)
      vkDestroyInstance(instance, NULL);
  }

  return result;
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
