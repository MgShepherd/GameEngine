#ifndef M_DEVICE_MANAGEMENT_H
#define M_DEVICE_MANAGEMENT_H

#include "instance.h"
#include "result.h"
#include <vulkan/vulkan_core.h>

#ifndef NUM_REQUIRED_QUEUES
#define NUM_REQUIRED_QUEUES 2
#endif

struct M_Device {
  VkDevice vk_device;
  VkPhysicalDevice physical_device;
  VkQueue graphics_queue;
  VkQueue present_queue;
};

struct M_QueueFamilyIndices {
  uint32_t graphics;
  uint32_t present;
};

struct M_QueueFamilyIndices m_physical_device_get_queue_families(VkPhysicalDevice device,
                                                                 const struct M_Instance *instance);

enum M_Result m_physical_device_find(M_Instance *instance);
enum M_Result m_device_create(M_Instance *instance);
void m_device_destroy(struct M_Device *device);

#endif
