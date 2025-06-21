#include "buffer_management.h"
#include "instance_private.h"
#include "logger.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "vk_utils.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

VkResult create_vk_buffer(struct M_Instance *instance, uint32_t size) {
  const VkBufferCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  return vkCreateBuffer(instance->device.vk_device, &create_info, NULL, &instance->buffer.vk_buffer);
}

uint32_t find_suitable_mem_type(VkPhysicalDevice physical_device, uint32_t type_filter,
                                VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  return UINT32_MAX;
}

enum M_Result create_vk_buffer_memory(struct M_Instance *instance, VkPhysicalDevice physical_device) {
  enum M_Result result = M_SUCCESS;

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(instance->device.vk_device, instance->buffer.vk_buffer, &mem_reqs);

  uint32_t idx = find_suitable_mem_type(physical_device, mem_reqs.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  if (idx == UINT32_MAX)
    result = m_result_process(M_VULKAN_INIT_ERR, "Unable to find suitable memory for buffer");

  const VkMemoryAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = mem_reqs.size,
      .memoryTypeIndex = idx,
  };

  vk_return_result_if_err(
      vkAllocateMemory(instance->device.vk_device, &allocate_info, NULL, &instance->buffer.vk_memory));
  return result;
}

enum M_Result m_buffer_create(struct M_Instance *instance, VkPhysicalDevice physical_device,
                              const struct M_Vertex *data, uint32_t num_elements) {
  enum M_Result result = M_SUCCESS;
  const VkDeviceSize buffer_size = sizeof(struct M_Vertex) * num_elements;

  vk_return_result_if_err(create_vk_buffer(instance, buffer_size));
  return_result_if_err(create_vk_buffer_memory(instance, physical_device));

  VkResult vk_result =
      vkBindBufferMemory(instance->device.vk_device, instance->buffer.vk_buffer, instance->buffer.vk_memory, 0);
  vk_return_result_if_err(vk_result);

  void *mapped_memory;
  vk_result = vkMapMemory(instance->device.vk_device, instance->buffer.vk_memory, 0, buffer_size, 0, &mapped_memory);
  vk_return_result_if_err(vk_result);
  memcpy(mapped_memory, data, buffer_size);
  vkUnmapMemory(instance->device.vk_device, instance->buffer.vk_memory);

  instance->buffer.num_elements = num_elements;

  return result;
}

void m_buffer_destroy(M_Instance *instance) {
  if (instance->buffer.vk_buffer != NULL) {
    vkDestroyBuffer(instance->device.vk_device, instance->buffer.vk_buffer, NULL);
  }
  if (instance->buffer.vk_memory != NULL) {
    vkFreeMemory(instance->device.vk_device, instance->buffer.vk_memory, NULL);
  }
}
