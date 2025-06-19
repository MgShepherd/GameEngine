#ifndef BUFFER_MANAGEMENT_H
#define BUFFER_MANAGEMENT_H

#include "instance.h"
#include "shader_management.h"
#include <vulkan/vulkan_core.h>

struct M_Buffer {
  VkBuffer vk_buffer;
  VkDeviceMemory vk_memory;
  uint32_t num_elements;
};

enum M_Result m_buffer_create(struct M_Instance *instance, VkPhysicalDevice physical_device,
                              const struct M_Vertex *data, uint32_t num_elements);
void m_buffer_destroy(M_Instance *instance);

#endif // !BUFFER_MANAGEMENT_H
