#ifndef VK_INSTANCE_HELPER_PROPERTIES_H
#define VK_INSTANCE_HELPER_PROPERTIES_H

#include "instance.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

enum M_Result vk_instance_create(VkInstance *vk_instance, const M_InstanceOptions *instance_options);
void vk_instance_destroy(VkInstance instance);

#endif // !VK_INSTANCE_HELPER_PROPERTIES_H
