#include "window.h"
#include "input.h"
#include "instance_private.h"
#include "logger.h"
#include "m_utils.h"
#include "result.h"
#include "result_utils.h"
#include "window_private.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void glfw_error_callback(int error, const char *description) { m_result_process(M_WINDOW_INIT_ERR, description); }

enum M_Result m_window_create(M_Window **window, const char *title, int width, int height) {
  enum M_Result result = M_SUCCESS;
  m_logger_init(M_INFO);

  glfwSetErrorCallback(glfw_error_callback);

  if (glfwInit() == GLFW_FALSE) {
    result = m_result_process(M_WINDOW_INIT_ERR, "Unable to initialise GLFW");
    m_window_destroy(*window);
  }

  *window = malloc(sizeof(struct M_Window));
  return_result_if_null_clean(*window, M_MEMORY_ALLOC_ERR, "Unable to allocate memory", m_window_destroy, *window);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  (*window)->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
  (*window)->width = width;
  (*window)->height = height;

  return_result_if_null_clean((*window)->glfw_window, M_WINDOW_INIT_ERR, "Unable to create GLFW Window",
                              m_window_destroy, *window);

  m_input_init(*window);
  m_logger_info("Successfully initialised M_Window");

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
  if (window != NULL) {
    if (window->glfw_window != NULL) {
      glfwSetWindowShouldClose(window->glfw_window, true);
      glfwDestroyWindow(window->glfw_window);
    }
    free(window);
  }
  glfwTerminate();
}

enum M_Result m_window_surface_create(const M_Window *window, struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  assert(instance != NULL && instance->vk_instance != NULL);
  VkResult vk_result = glfwCreateWindowSurface(instance->vk_instance, window->glfw_window, NULL, &instance->vk_surface);
  vk_return_result_if_err(vk_result);
  return result;
}

void m_window_surface_destroy(M_Instance *instance) {
  assert(instance != NULL);
  if (instance->vk_surface != NULL) {
    vkDestroySurfaceKHR(instance->vk_instance, instance->vk_surface, NULL);
  }
}
