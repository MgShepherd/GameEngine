#include "vk_device_management.h"
#include "instance_private.h"
#include "logger.h"
#include "result.h"
#include "vk_utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_REQUIRED_QUEUES = 2;
const uint32_t NUM_OPTIONAL_EXTENSIONS = 1;
const char *OPTIONAL_EXTENSIONS[] = {"VK_KHR_portability_subset"};

struct M_QueueFamilyIndices {
  uint32_t graphics;
  uint32_t present;
};

struct M_QueueFamilyIndices vk_physical_device_get_queue_families(VkPhysicalDevice device, const struct M_Instance *instance) {
  struct M_QueueFamilyIndices indices = {.graphics = UINT32_MAX, .present = UINT32_MAX};
  uint32_t remaining_queues_to_find = NUM_REQUIRED_QUEUES;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
  VkQueueFamilyProperties queue_family_properties[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

  VkBool32 has_present_support = false;

  for (uint32_t i = 0; i < queue_family_count; i++) {
    if (indices.graphics == UINT32_MAX && queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
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

bool vk_physical_device_is_suitable(VkPhysicalDevice device, const struct M_Instance *instance) {
  struct M_QueueFamilyIndices indices = vk_physical_device_get_queue_families(device, instance);

  return indices.graphics != UINT32_MAX && indices.present != UINT32_MAX;
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

void add_device_queue_create_info(VkDeviceQueueCreateInfo *create_infos, uint32_t *create_info_idx, uint32_t queue_index, float *priority) {
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

enum M_Result vk_physical_device_find(VkPhysicalDevice *physical_device, const struct M_Instance *instance) {
  assert(instance->vk_instance != NULL && instance->vk_surface != NULL);

  enum M_Result result = M_SUCCESS;
  *physical_device = NULL;

  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance->vk_instance, &device_count, NULL);
  VkPhysicalDevice available_devices[device_count];
  vkEnumeratePhysicalDevices(instance->vk_instance, &device_count, available_devices);

  for (uint32_t i = 0; i < device_count; i++) {
    if (vk_physical_device_is_suitable(available_devices[i], instance)) {
      *physical_device = available_devices[i];
      break;
    }
  }

  if (*physical_device == NULL) {
    result = m_result_process(M_VULKAN_INIT_ERR, "Unable to find a Graphics Device with required features");
  }

  return result;
}

enum M_Result vk_device_create(struct M_Instance *instance, VkPhysicalDevice physical_device) {
  assert(physical_device != NULL && instance->vk_instance != NULL && instance->vk_surface != NULL);
  enum M_Result result = M_SUCCESS;

  struct M_QueueFamilyIndices queue_families = vk_physical_device_get_queue_families(physical_device, instance);
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo device_queue_create_infos[NUM_REQUIRED_QUEUES];
  uint32_t current_queue_idx = 0;
  add_device_queue_create_info(device_queue_create_infos, &current_queue_idx, queue_families.graphics, &queue_priority);
  add_device_queue_create_info(device_queue_create_infos, &current_queue_idx, queue_families.present, &queue_priority);

  VkPhysicalDeviceFeatures device_features = {};

  uint32_t num_device_extensions = 0;
  const char **extensions = vk_device_get_extensions(physical_device, &num_device_extensions);
  if (extensions == NULL) {
    result = m_result_process(M_VULKAN_INIT_ERR, "Unable to load required device extensions");
    goto device_cleanup;
  }

  VkDeviceCreateInfo device_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = current_queue_idx,
      .pQueueCreateInfos = device_queue_create_infos,
      .pEnabledFeatures = &device_features,
      .enabledExtensionCount = num_device_extensions,
      .ppEnabledExtensionNames = extensions,
  };

  result = process_vulkan_result(vkCreateDevice(physical_device, &device_create_info, NULL, &instance->vk_device));

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
