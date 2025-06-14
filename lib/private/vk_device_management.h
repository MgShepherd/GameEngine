#ifndef VK_DEVICE_MANAGEMENT_H
#define VK_DEVICE_MANAGEMENT_H

#include "instance.h"
#include "result.h"
#include <vulkan/vulkan_core.h>

struct M_Device {
  VkDevice vk_device;
  VkQueue graphics_queue;
  VkQueue present_queue;
};

enum M_Result vk_physical_device_find(VkPhysicalDevice *physical_device, const M_Instance *instance);
enum M_Result vk_device_create(M_Instance *instance, VkPhysicalDevice physical_device);
void vk_device_destroy(struct M_Device *device);

#endif // !VK_DEVICE_MANAGEMENT_H
