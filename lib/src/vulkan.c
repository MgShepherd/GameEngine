#include "vulkan.h"
#include "GLFW/glfw3.h"
#include "logger.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_ADDITIONAL_EXTENSIONS = 1;

void process_vulkan_result(VkResult result) {
  if (result != VK_SUCCESS) {
    m_logger_error("Vulkan Error: %s", string_VkResult(result));
  }
}

void get_required_extensions(const char ***extensions, uint32_t *num_extensions) {
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(num_extensions);
  // TODO: Handle null value here - should maybe check vulkan is supported first

  *num_extensions += NUM_ADDITIONAL_EXTENSIONS;
  *extensions = malloc(*num_extensions * sizeof(char *));
  // TODO: Handle null value here
  memcpy(*extensions, glfw_extensions, (*num_extensions - NUM_ADDITIONAL_EXTENSIONS) * sizeof(char *));
  (*extensions)[*num_extensions - 1] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
}

void m_vulkan_init(VkInstance *instance, const char *app_name) {
  const VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = app_name,
      .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
  };

  VkInstanceCreateInfo instance_create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = 0,
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
  };

  uint32_t num_extensions = 0;
  const char **extensions = NULL;
  get_required_extensions(&extensions, &num_extensions);

  instance_create_info.enabledExtensionCount = num_extensions;
  instance_create_info.ppEnabledExtensionNames = extensions;

  const VkResult result = vkCreateInstance(&instance_create_info, NULL, instance);
  process_vulkan_result(result);

  // TODO: Improve error processing
  if (result == VK_SUCCESS) {
    m_logger_info("Successfully initialised vulkan");
  } else {
    m_logger_error("Unable to initialise vulkan");
  }

  if (extensions != NULL)
    free(extensions);
}
