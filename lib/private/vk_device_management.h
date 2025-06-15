#ifndef VK_DEVICE_MANAGEMENT_H
#define VK_DEVICE_MANAGEMENT_H

#include "instance.h"
#include "result.h"
#include <vulkan/vulkan_core.h>

#ifndef NUM_REQUIRED_QUEUES
#define NUM_REQUIRED_QUEUES 2
#endif

struct M_Device {
  VkDevice vk_device;
  VkQueue graphics_queue;
  VkQueue present_queue;
};

struct M_QueueFamilyIndices {
  uint32_t graphics;
  uint32_t present;
};

struct M_QueueFamilyIndices vk_physical_device_get_queue_families(VkPhysicalDevice device,
                                                                  const struct M_Instance *instance);

enum M_Result vk_physical_device_find(VkPhysicalDevice *physical_device, const M_Instance *instance);
enum M_Result vk_device_create(M_Instance *instance, VkPhysicalDevice physical_device);
void vk_device_destroy(struct M_Device *device);

#endif // !VK_DEVICE_MANAGEMENT_H
