#include "buffer_management.h"
#include "instance_private.h"
#include "logger.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "vk_utils.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

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

enum M_Result copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size, struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  VkCommandPool command_pool;
  result = create_command_pool(&command_pool, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, instance);
  return_result_if_err(result);

  const VkCommandBufferAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandPool = command_pool,
      .commandBufferCount = 1,
  };

  VkCommandBuffer command_buffer;
  VkResult vk_result = vkAllocateCommandBuffers(instance->device.vk_device, &allocate_info, &command_buffer);
  vk_return_result_if_err_clean(vk_result, vkDestroyCommandPool, instance->device.vk_device, command_pool, NULL);

  const VkCommandBufferBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  vkBeginCommandBuffer(command_buffer, &begin_info);
  const VkBufferCopy copy_region = {
      .size = size,
  };
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
  vk_result = vkEndCommandBuffer(command_buffer);
  vk_return_result_if_err_clean(vk_result, vkDestroyCommandPool, instance->device.vk_device, command_pool, NULL);

  const VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers = &command_buffer,
  };

  vk_result = vkQueueSubmit(instance->device.graphics_queue, 1, &submit_info, NULL);
  vk_return_result_if_err_clean(vk_result, vkDestroyCommandPool, instance->device.vk_device, command_pool, NULL);
  vk_result = vkQueueWaitIdle(instance->device.graphics_queue);
  vkDestroyCommandPool(instance->device.vk_device, command_pool, NULL);

  return result;
}

enum M_Result create_buffer(const M_Instance *instance, VkDeviceSize size, VkBufferUsageFlags usage,
                            VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory) {
  enum M_Result result = M_SUCCESS;
  const VkBufferCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  vk_return_result_if_err(vkCreateBuffer(instance->device.vk_device, &create_info, NULL, buffer));

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(instance->device.vk_device, *buffer, &mem_requirements);
  const uint32_t type_idx =
      find_suitable_mem_type(instance->device.physical_device, mem_requirements.memoryTypeBits, properties);
  if (type_idx == UINT32_MAX) {
    return m_result_process(M_VULKAN_INIT_ERR, "Unable to find suitable memory");
  }

  const VkMemoryAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = mem_requirements.size,
      .memoryTypeIndex = type_idx,
  };

  vk_return_result_if_err(vkAllocateMemory(instance->device.vk_device, &allocate_info, NULL, memory));
  vk_return_result_if_err(vkBindBufferMemory(instance->device.vk_device, *buffer, *memory, 0));

  return result;
}

void buffer_create_free(const struct M_Instance *instance, VkBuffer buffer, VkDeviceMemory memory) {
  vkDestroyBuffer(instance->device.vk_device, buffer, NULL);
  vkFreeMemory(instance->device.vk_device, memory, NULL);
}

enum M_Result m_buffer_create(struct M_Buffer *buffer, struct M_Instance *instance, const void *data,
                              VkDeviceSize buffer_size, VkBufferUsageFlags usage) {
  enum M_Result result = M_SUCCESS;

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;

  result = create_buffer(instance, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer,
                         &staging_buffer_memory);

  void *mapped_memory;
  const VkResult vk_result =
      vkMapMemory(instance->device.vk_device, staging_buffer_memory, 0, buffer_size, 0, &mapped_memory);
  return_result_if_err_clean(result, buffer_create_free, instance, staging_buffer, staging_buffer_memory);
  memcpy(mapped_memory, data, buffer_size);
  vkUnmapMemory(instance->device.vk_device, staging_buffer_memory);

  result = create_buffer(instance, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->vk_buffer, &buffer->vk_memory);
  return_result_if_err_clean(result, buffer_create_free, instance, staging_buffer, staging_buffer_memory);

  result = copy_buffer(staging_buffer, buffer->vk_buffer, buffer_size, instance);

  buffer_create_free(instance, staging_buffer, staging_buffer_memory);

  return result;
}

void m_buffer_destroy(struct M_Buffer *buffer, struct M_Instance *instance) {
  if (buffer->vk_buffer != NULL) {
    vkDestroyBuffer(instance->device.vk_device, buffer->vk_buffer, NULL);
  }
  if (buffer->vk_memory != NULL) {
    vkFreeMemory(instance->device.vk_device, buffer->vk_memory, NULL);
  }
}
