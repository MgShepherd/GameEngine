#ifndef BUFFER_MANAGEMENT_H
#define BUFFER_MANAGEMENT_H

#include "instance.h"
#include <vulkan/vulkan_core.h>

struct M_Buffer {
  VkBuffer vk_buffer;
  VkDeviceMemory vk_memory;
  uint32_t num_elements;
};

enum M_Result m_buffer_create(struct M_Buffer *buffer, const struct M_Instance *instance, const void *data,
                              VkDeviceSize buffer_size, VkBufferUsageFlags usage);
void m_buffer_destroy(struct M_Buffer *buffer, const struct M_Instance *instance);

#endif // !BUFFER_MANAGEMENT_H
