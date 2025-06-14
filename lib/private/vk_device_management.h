#ifndef VK_DEVICE_MANAGEMENT_H
#define VK_DEVICE_MANAGEMENT_H

#include "instance.h"
#include "result.h"
#include <vulkan/vulkan_core.h>

enum M_Result vk_physical_device_find(VkPhysicalDevice *physical_device, const M_Instance *instance);
enum M_Result vk_device_create(M_Instance *instance, VkPhysicalDevice physical_device);
void vk_device_destroy(VkDevice device);

#endif // !VK_DEVICE_MANAGEMENT_H
