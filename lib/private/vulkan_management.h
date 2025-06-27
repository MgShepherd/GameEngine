#ifndef M_VULKAN_MANAGEMENT_H
#define M_VULKAN_MANAGEMENT_H

#include "instance.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

enum M_Result m_vulkan_instance_create(VkInstance *vk_instance, const M_InstanceOptions *instance_options);
void m_vulkan_instance_destroy(VkInstance instance);

#endif
