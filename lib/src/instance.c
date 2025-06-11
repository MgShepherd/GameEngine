#include "instance.h"
#include "GLFW/glfw3.h"
#include "logger.h"
#include "result.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_ADDITIONAL_EXTENSIONS = 1;

struct M_Instance {
  VkInstance vk_instance;
};

enum M_Result process_vulkan_result(VkResult result) {
  if (result != VK_SUCCESS) {
    return m_result_process(M_VULKAN_INIT_ERR, string_VkResult(result));
  }

  return M_SUCCESS;
}

bool all_extensions_supported(const VkExtensionProperties *available_extensions, uint32_t num_available_extensions, const char **required_extensions, uint32_t num_required_extensions) {
  bool found = false;
  for (uint32_t i = 0; i < num_required_extensions; i++) {
    found = false;
    for (uint32_t j = 0; j < num_available_extensions; j++) {
      if (strcmp(available_extensions[j].extensionName, required_extensions[i]) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
      m_logger_error("Unsupported instance extension: %s", required_extensions[i]);
      return false;
    }
  }

  return true;
}

const char **get_required_extensions(uint32_t *num_extensions) {
  uint32_t num_available_extensions = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, NULL);
  VkExtensionProperties available_extensions[num_available_extensions];
  const enum M_Result result = process_vulkan_result(vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, available_extensions));
  if (result != M_SUCCESS)
    return NULL;

  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(num_extensions);
  *num_extensions += NUM_ADDITIONAL_EXTENSIONS;
  const char **extensions = malloc(*num_extensions * sizeof(char *));
  if (extensions == NULL) {
    return NULL;
  }

  memcpy(extensions, glfw_extensions, (*num_extensions - NUM_ADDITIONAL_EXTENSIONS) * sizeof(char *));
  extensions[*num_extensions - 1] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;

  if (!all_extensions_supported(available_extensions, num_available_extensions, extensions, *num_extensions)) {
    free(extensions);
    return NULL;
  }

  return extensions;
}

enum M_Result m_instance_create(struct M_Instance **instance, const char *app_name) {
  enum M_Result result = M_SUCCESS;

  *instance = malloc(sizeof(struct M_Instance));
  if (*instance == NULL) {
    result = m_result_process(M_MEMORY_ALLOC_ERR, "Unable to allocate required memory for M_Instance struct");
    goto instance_init_cleanup;
  }

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
  const char **extensions = get_required_extensions(&num_extensions);
  if (extensions == NULL) {
    result = m_result_process(M_VULKAN_INIT_ERR, "Unable to load required instance extensions");
    goto instance_init_cleanup;
  }

  instance_create_info.enabledExtensionCount = num_extensions;
  instance_create_info.ppEnabledExtensionNames = extensions;

  result = process_vulkan_result(vkCreateInstance(&instance_create_info, NULL, &(*instance)->vk_instance));
  if (result != M_SUCCESS)
    goto instance_init_cleanup;

  m_logger_info("Successfully initialised M_Instance");

instance_init_cleanup:
  if (extensions != NULL)
    free(extensions);
  if (result != M_SUCCESS) {
    m_instance_destroy(*instance);
    *instance = NULL;
  }

  return result;
}

void m_instance_destroy(M_Instance *instance) {
  if (instance != NULL) {
    if (instance->vk_instance != NULL) {
      vkDestroyInstance(instance->vk_instance, NULL);
    }
    free(instance);
  }
}
