#include "uniform_management.h"
#include "buffer_management.h"
#include "cglm/cam.h"
#include "cglm/util.h"
#include "instance_private.h"
#include "m_utils.h"
#include "render_management.h"
#include "result.h"
#include "result_utils.h"
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

enum M_Result create_descriptor_pool(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  const VkDescriptorPoolSize pool_size = {
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = MAX_FRAMES_IN_FLIGHT,
  };

  const VkDescriptorPoolCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .poolSizeCount = 1,
      .pPoolSizes = &pool_size,
      .maxSets = MAX_FRAMES_IN_FLIGHT,
  };

  VkResult vk_result =
      vkCreateDescriptorPool(instance->device.vk_device, &create_info, NULL, &instance->uniforms.descriptor_pool);
  vk_return_result_if_err(vk_result);

  return result;
}

enum M_Result create_descriptor_sets(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  instance->uniforms.descriptor_sets = malloc(sizeof(VkDescriptorSet) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->uniforms.descriptor_sets, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  const VkDescriptorSetLayout layouts[] = {instance->pipeline.descriptor_layout, instance->pipeline.descriptor_layout,
                                           instance->pipeline.descriptor_layout};
  const VkDescriptorSetAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = instance->uniforms.descriptor_pool,
      .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
      .pSetLayouts = layouts,
  };

  VkResult vk_result =
      vkAllocateDescriptorSets(instance->device.vk_device, &allocate_info, instance->uniforms.descriptor_sets);
  vk_return_result_if_err(vk_result);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    const VkDescriptorBufferInfo buffer_info = {
        .buffer = instance->uniforms.buffers[i].vk_buffer,
        .offset = 0,
        .range = sizeof(struct M_UniformBufferObject),
    };

    const VkWriteDescriptorSet descriptor_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = instance->uniforms.descriptor_sets[i],
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pBufferInfo = &buffer_info,
    };

    vkUpdateDescriptorSets(instance->device.vk_device, 1, &descriptor_write, 0, NULL);
  }

  return result;
}

enum M_Result m_uniforms_create(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  instance->uniforms.buffers = malloc(sizeof(struct M_Buffer) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->uniforms.buffers, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");
  instance->uniforms.mapped_memories = malloc(sizeof(void *) * MAX_FRAMES_IN_FLIGHT);
  return_result_if_null(instance->uniforms.mapped_memories, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  const VkDeviceSize buffer_size = sizeof(struct M_UniformBufferObject);

  struct M_UniformBufferObject ubo = {};
  glm_ortho(0.0f, instance->swapchain.extent.width, 0.0f, instance->swapchain.extent.height, -1.0f, 1.0f, ubo.proj);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    m_buffer_create(&instance->uniforms.buffers[i], instance, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkMapMemory(instance->device.vk_device, instance->uniforms.buffers[i].vk_memory, 0, buffer_size, 0,
                &instance->uniforms.mapped_memories[i]);

    memcpy(instance->uniforms.mapped_memories[i], &ubo, sizeof(struct M_UniformBufferObject));
  }

  return_result_if_err(create_descriptor_pool(instance));
  return_result_if_err(create_descriptor_sets(instance));

  return result;
}

void m_uniforms_destroy(struct M_Instance *instance) {
  if (instance->uniforms.descriptor_pool != NULL) {
    vkDestroyDescriptorPool(instance->device.vk_device, instance->uniforms.descriptor_pool, NULL);
    instance->uniforms.descriptor_pool = NULL;
  }
  if (instance->uniforms.buffers != NULL) {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      m_buffer_destroy(&instance->uniforms.buffers[i], instance);
    }
    free(instance->uniforms.buffers);
    instance->uniforms.buffers = NULL;
  }
  if (instance->uniforms.mapped_memories != NULL) {
    free(instance->uniforms.mapped_memories);
    instance->uniforms.mapped_memories = NULL;
  }
}
