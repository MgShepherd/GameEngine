#include "vk_device_management.h"
#include "logger.h"
#include "result.h"
#include "vk_utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_REQUIRED_QUEUES = 1;
const uint32_t NUM_OPTIONAL_EXTENSIONS = 1;
const char *OPTIONAL_EXTENSIONS[] = {"VK_KHR_portability_subset"};

struct M_QueueFamilyIndices {
  uint32_t graphics;
};

struct M_QueueFamilyIndices vk_physical_device_get_queue_families(VkPhysicalDevice device) {
  struct M_QueueFamilyIndices indices = {.graphics = UINT32_MAX};
  uint32_t remaining_queues_to_find = NUM_REQUIRED_QUEUES;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
  VkQueueFamilyProperties queue_family_properties[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

  for (uint32_t i = 0; i < queue_family_count; i++) {
    if (indices.graphics == UINT32_MAX && queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = i;
      remaining_queues_to_find--;
    }

    if (remaining_queues_to_find == 0)
      break;
  }

  return indices;
}

bool vk_physical_device_is_suitable(VkPhysicalDevice device) {
  struct M_QueueFamilyIndices indices = vk_physical_device_get_queue_families(device);

  return indices.graphics != UINT32_MAX;
}

const char **vk_device_get_extensions(VkPhysicalDevice device, uint32_t *num_extensions) {
  uint32_t num_available_extensions = 0;
  vkEnumerateDeviceExtensionProperties(device, NULL, &num_available_extensions, NULL);
  VkExtensionProperties available_extensions[num_available_extensions];
  vkEnumerateDeviceExtensionProperties(device, NULL, &num_available_extensions, available_extensions);

  const char **extensions = malloc(NUM_OPTIONAL_EXTENSIONS * sizeof(char *));
  if (extensions == NULL) {
    return NULL;
  }

  *num_extensions = 0;
  for (uint32_t i = 0; i < NUM_OPTIONAL_EXTENSIONS; i++) {
    for (uint32_t j = 0; j < num_available_extensions; j++) {
      if (strcmp(OPTIONAL_EXTENSIONS[i], available_extensions[j].extensionName)) {
        extensions[(*num_extensions)++] = OPTIONAL_EXTENSIONS[i];
        break;
      }
    }
  }

  return extensions;
}

enum M_Result vk_physical_device_find(VkPhysicalDevice *physical_device, VkInstance instance) {
  assert(instance != NULL);

  enum M_Result result = M_SUCCESS;
  *physical_device = NULL;

  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance, &device_count, NULL);
  VkPhysicalDevice available_devices[device_count];
  vkEnumeratePhysicalDevices(instance, &device_count, available_devices);

  for (uint32_t i = 0; i < device_count; i++) {
    if (vk_physical_device_is_suitable(available_devices[i])) {
      *physical_device = available_devices[i];
      break;
    }
  }

  if (*physical_device == NULL) {
    result = m_result_process(M_VULKAN_INIT_ERR, "Unable to find a Graphics Device with required features");
  }

  return result;
}

enum M_Result vk_device_create(VkDevice *device, VkPhysicalDevice physical_device) {
  assert(physical_device != NULL);
  enum M_Result result = M_SUCCESS;

  struct M_QueueFamilyIndices queue_families = vk_physical_device_get_queue_families(physical_device);
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo queue_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = queue_families.graphics,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
  };

  VkPhysicalDeviceFeatures device_features = {};

  uint32_t num_device_extensions = 0;
  const char **extensions = vk_device_get_extensions(physical_device, &num_device_extensions);
  if (extensions == NULL) {
    result = m_result_process(M_VULKAN_INIT_ERR, "Unable to load required device extensions");
    goto device_cleanup;
  }

  VkDeviceCreateInfo device_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queue_create_info,
      .pEnabledFeatures = &device_features,
      .enabledExtensionCount = num_device_extensions,
      .ppEnabledExtensionNames = extensions,
  };

  result = process_vulkan_result(vkCreateDevice(physical_device, &device_create_info, NULL, device));

device_cleanup:
  if (extensions != NULL) {
    free(extensions);
  }
  return result;
}

void vk_device_destroy(VkDevice device) {
  if (device != NULL) {
    vkDestroyDevice(device, NULL);
  }
}
