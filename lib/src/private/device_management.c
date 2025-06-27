#include "device_management.h"
#include "instance_private.h"
#include "logger.h"
#include "m_utils.h"
#include "result.h"
#include "result_utils.h"
#include "swap_chain_management.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_REQUIRED_EXTENSIONS = 1;
const uint32_t NUM_OPTIONAL_EXTENSIONS = 1;
const char *OPTIONAL_EXTENSIONS[] = {"VK_KHR_portability_subset"};
const char *REQUIRED_EXTENSIONS[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

bool m_physical_device_supports_required_extensions(VkPhysicalDevice device) {
  uint32_t num_available_extensions = 0;
  vk_return_false_if_err(vkEnumerateDeviceExtensionProperties(device, NULL, &num_available_extensions, NULL));
  VkExtensionProperties available_extensions[num_available_extensions];
  vk_return_false_if_err(
      vkEnumerateDeviceExtensionProperties(device, NULL, &num_available_extensions, available_extensions));

  bool found = false;
  for (uint32_t i = 0; i < NUM_REQUIRED_EXTENSIONS; i++) {
    found = false;
    for (uint32_t j = 0; j < num_available_extensions; j++) {
      if (strcmp(REQUIRED_EXTENSIONS[i], available_extensions[j].extensionName) == 0) {
        found = true;
        break;
      }
    }

    if (!found)
      return false;
  }

  return true;
}

bool m_physical_device_is_suitable(VkPhysicalDevice device, const struct M_Instance *instance) {
  struct M_QueueFamilyIndices indices = m_physical_device_get_queue_families(device, instance);
  if (indices.graphics == UINT32_MAX || indices.present == UINT32_MAX)
    return false;
  if (!m_physical_device_supports_required_extensions(device))
    return false;

  struct M_SwapChainSupport swap_support;
  return_false_if_err(m_swap_chain_get_device_support(&swap_support, device, instance));

  bool swap_chain_supported = swap_support.num_formats != 0 && swap_support.num_present_modes != 0;
  m_swap_chain_support_destroy(&swap_support);
  return swap_chain_supported;
}

void add_extension_if_found(const char *extension_name, VkExtensionProperties *available_extensions,
                            uint32_t num_available_extensions, const char **found_extensions,
                            uint32_t *num_found_extensions) {
  for (uint32_t j = 0; j < num_available_extensions; j++) {
    if (strcmp(extension_name, available_extensions[j].extensionName) == 0) {
      found_extensions[(*num_found_extensions)++] = extension_name;
      return;
    }
  }
}

const char **vk_device_get_extensions(VkPhysicalDevice device, uint32_t *num_extensions) {
  uint32_t num_available_extensions = 0;
  vk_return_null_if_err(vkEnumerateDeviceExtensionProperties(device, NULL, &num_available_extensions, NULL));
  VkExtensionProperties available_extensions[num_available_extensions];
  vk_return_null_if_err(
      vkEnumerateDeviceExtensionProperties(device, NULL, &num_available_extensions, available_extensions));

  const char **extensions = malloc((NUM_OPTIONAL_EXTENSIONS + NUM_REQUIRED_EXTENSIONS) * sizeof(char *));
  return_null_if_null(extensions, M_MEMORY_ALLOC_ERR, "Unable to allocate memory for extensions");

  *num_extensions = 0;
  for (uint32_t i = 0; i < NUM_OPTIONAL_EXTENSIONS; i++) {
    add_extension_if_found(OPTIONAL_EXTENSIONS[i], available_extensions, num_available_extensions, extensions,
                           num_extensions);
  }
  for (uint32_t i = 0; i < NUM_REQUIRED_EXTENSIONS; i++) {
    add_extension_if_found(REQUIRED_EXTENSIONS[i], available_extensions, num_available_extensions, extensions,
                           num_extensions);
  }

  return extensions;
}

void add_device_queue_create_info(VkDeviceQueueCreateInfo *create_infos, uint32_t *create_info_idx,
                                  uint32_t queue_index, float *priority) {
  for (uint32_t i = 0; i < *create_info_idx; i++) {
    if (create_infos[i].queueFamilyIndex == queue_index)
      return;
  }

  create_infos[(*create_info_idx)++] = (VkDeviceQueueCreateInfo){
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = queue_index,
      .queueCount = 1,
      .pQueuePriorities = priority,
  };
}

struct M_QueueFamilyIndices m_physical_device_get_queue_families(VkPhysicalDevice device,
                                                                 const struct M_Instance *instance) {
  struct M_QueueFamilyIndices indices = {.graphics = UINT32_MAX, .present = UINT32_MAX};
  uint32_t remaining_queues_to_find = NUM_REQUIRED_QUEUES;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
  VkQueueFamilyProperties queue_family_properties[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

  VkBool32 has_present_support = false;

  for (uint32_t i = 0; i < queue_family_count; i++) {
    if (indices.graphics == UINT32_MAX && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      indices.graphics = i;
      remaining_queues_to_find--;
    }
    if (indices.present == UINT32_MAX) {
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance->vk_surface, &has_present_support);
      if (has_present_support) {
        indices.present = i;
        remaining_queues_to_find--;
      }
    }

    if (remaining_queues_to_find == 0)
      break;
  }

  return indices;
}

enum M_Result m_physical_device_find(struct M_Instance *instance) {
  assert(instance->vk_instance != NULL && instance->vk_surface != NULL);
  instance->device.physical_device = NULL;

  uint32_t device_count = 0;
  vk_return_result_if_err(vkEnumeratePhysicalDevices(instance->vk_instance, &device_count, NULL));
  VkPhysicalDevice available_devices[device_count];
  vk_return_result_if_err(vkEnumeratePhysicalDevices(instance->vk_instance, &device_count, available_devices));

  for (uint32_t i = 0; i < device_count; i++) {
    m_logger_info("Looking at device index %d", i);
    if (m_physical_device_is_suitable(available_devices[i], instance)) {
      m_logger_info("Found device");
      instance->device.physical_device = available_devices[i];
      break;
    }
  }

  enum M_Result result = M_SUCCESS;
  return_result_if_null(instance->device.physical_device, M_VULKAN_INIT_ERR,
                        "Unable to find a Graphics Device with required features");

  return result;
}

enum M_Result m_device_create(struct M_Instance *instance) {
  assert(instance->device.physical_device != NULL && instance->vk_instance != NULL && instance->vk_surface != NULL);
  enum M_Result result = M_SUCCESS;

  struct M_QueueFamilyIndices queue_families =
      m_physical_device_get_queue_families(instance->device.physical_device, instance);
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo device_queue_create_infos[NUM_REQUIRED_QUEUES];
  uint32_t current_queue_idx = 0;
  add_device_queue_create_info(device_queue_create_infos, &current_queue_idx, queue_families.graphics, &queue_priority);
  add_device_queue_create_info(device_queue_create_infos, &current_queue_idx, queue_families.present, &queue_priority);

  VkPhysicalDeviceFeatures device_features = {};

  uint32_t num_device_extensions = 0;
  const char **extensions = vk_device_get_extensions(instance->device.physical_device, &num_device_extensions);
  return_result_if_null(extensions, M_VULKAN_INIT_ERR, "Unable to load required device extensions");

  VkDeviceCreateInfo device_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = current_queue_idx,
      .pQueueCreateInfos = device_queue_create_infos,
      .pEnabledFeatures = &device_features,
      .enabledExtensionCount = num_device_extensions,
      .ppEnabledExtensionNames = extensions,
  };

  result = process_vulkan_result(
      vkCreateDevice(instance->device.physical_device, &device_create_info, NULL, &instance->device.vk_device));

  vkGetDeviceQueue(instance->device.vk_device, queue_families.graphics, 0, &instance->device.graphics_queue);
  vkGetDeviceQueue(instance->device.vk_device, queue_families.present, 0, &instance->device.present_queue);

  if (extensions != NULL) {
    free(extensions);
  }
  return result;
}

void m_device_destroy(struct M_Device *device) {
  if (device->vk_device != NULL) {
    vkDestroyDevice(device->vk_device, NULL);
    device->vk_device = NULL;
  }
}
