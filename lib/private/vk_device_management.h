#ifndef VK_DEVICE_MANAGEMENT_H
#define VK_DEVICE_MANAGEMENT_H

#include "result.h"
#include <vulkan/vulkan_core.h>

enum M_Result vk_physical_device_find(VkPhysicalDevice *physical_device, VkInstance instance);
enum M_Result vk_device_create(VkDevice *device, VkPhysicalDevice physical_device);
void vk_device_destroy(VkDevice device);

#endif // !VK_DEVICE_MANAGEMENT_H
