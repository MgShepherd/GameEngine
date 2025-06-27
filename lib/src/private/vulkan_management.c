#include "vulkan_management.h"
#include "GLFW/glfw3.h"
#include "debug_messenger.h"
#include "instance.h"
#include "logger.h"
#include "m_utils.h"
#include "result.h"
#include "result_utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_ADDITIONAL_EXTENSIONS = 3;
const uint32_t NUM_VALIDATION_LAYERS = 1;

bool all_extensions_supported(const VkExtensionProperties *available_extensions, uint32_t num_available_extensions,
                              const char **required_extensions, uint32_t num_required_extensions) {
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

bool all_layers_supported(const VkLayerProperties *available_layers, uint32_t num_available_layers,
                          const char **required_layers) {
  bool found = false;
  for (uint32_t i = 0; i < NUM_VALIDATION_LAYERS; i++) {
    found = false;
    for (uint32_t j = 0; j < num_available_layers; j++) {
      if (strcmp(available_layers[j].layerName, required_layers[i]) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
      m_logger_error("Unsupported instance validation layer: %s", required_layers[i]);
      return false;
    }
  }

  return true;
}

const char **get_required_extensions(uint32_t *num_extensions) {
  uint32_t num_available_extensions = 0;
  vk_return_null_if_err(vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, NULL));
  VkExtensionProperties available_extensions[num_available_extensions];
  vk_return_null_if_err(vkEnumerateInstanceExtensionProperties(NULL, &num_available_extensions, available_extensions));

  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(num_extensions);
  *num_extensions += NUM_ADDITIONAL_EXTENSIONS;
  const char **extensions = malloc(*num_extensions * sizeof(char *));
  return_null_if_null(extensions, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  memcpy(extensions, glfw_extensions, (*num_extensions - NUM_ADDITIONAL_EXTENSIONS) * sizeof(char *));
  extensions[*num_extensions - 1] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
  extensions[*num_extensions - 2] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  extensions[*num_extensions - 3] = "VK_KHR_get_physical_device_properties2";

  if (!all_extensions_supported(available_extensions, num_available_extensions, extensions, *num_extensions)) {
    free(extensions);
    return NULL;
  }

  return extensions;
}

const char **get_validation_layers() {
  uint32_t num_available_layers = 0;
  vk_return_null_if_err(vkEnumerateInstanceLayerProperties(&num_available_layers, NULL));
  VkLayerProperties available_layers[num_available_layers];
  vk_return_null_if_err(vkEnumerateInstanceLayerProperties(&num_available_layers, available_layers));

  const char **required_layers = malloc(NUM_VALIDATION_LAYERS * sizeof(char *));
  return_null_if_null(required_layers, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");
  required_layers[0] = "VK_LAYER_KHRONOS_validation";

  if (!all_layers_supported(available_layers, num_available_layers, required_layers)) {
    free(required_layers);
    return NULL;
  }

  return required_layers;
}

void cleanup_instance_create(const char **extensions, const char **validation_layers) {
  if (extensions != NULL)
    free(extensions);
  if (validation_layers != NULL)
    free(validation_layers);
}

enum M_Result m_vulkan_instance_create(VkInstance *vk_instance, const M_InstanceOptions *instance_options) {
  enum M_Result result = M_SUCCESS;
  const char **validation_layers = NULL;
  const char **extensions = NULL;

  const VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = instance_options->app_name,
      .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo instance_create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
  };

  uint32_t num_extensions = 0;
  extensions = get_required_extensions(&num_extensions);
  const char *extensions_failed_msg = "Unable to load required instance extensions";
  return_result_if_null_clean(extensions, M_VULKAN_INIT_ERR, extensions_failed_msg, cleanup_instance_create, extensions,
                              validation_layers);
  instance_create_info.enabledExtensionCount = num_extensions;
  instance_create_info.ppEnabledExtensionNames = extensions;

  VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info;
  if (instance_options->enable_debug) {
    validation_layers = get_validation_layers();
    const char *validations_failed_msg = "Unable to load requested validation layers";
    return_result_if_null_clean(validation_layers, M_VULKAN_INIT_ERR, validations_failed_msg, cleanup_instance_create,
                                extensions, validation_layers);

    instance_create_info.enabledLayerCount = NUM_VALIDATION_LAYERS;
    instance_create_info.ppEnabledLayerNames = validation_layers;

    m_debug_messenger_fill_create_info(&debug_messenger_create_info);
    instance_create_info.pNext = &debug_messenger_create_info;
    m_logger_info("Validation layers enabled");
  } else {
    instance_create_info.enabledLayerCount = 0;
  }

  VkResult vk_result = vkCreateInstance(&instance_create_info, NULL, vk_instance);
  vk_return_result_if_err_clean(vk_result, cleanup_instance_create, extensions, validation_layers);

  cleanup_instance_create(extensions, validation_layers);
  return result;
}

void m_vulkan_instance_destroy(VkInstance instance) {
  if (instance != NULL) {
    vkDestroyInstance(instance, NULL);
  }
}
