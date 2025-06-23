#ifndef UNIFORM_MANAGEMENT_H
#define UNIFORM_MANAGEMENT_H

#include "buffer_management.h"
#include "cglm/types.h"
#include "instance.h"
#include <vulkan/vulkan_core.h>

struct M_UniformBufferObject {
  mat4 proj;
};

struct M_Uniforms {
  struct M_Buffer *buffers;
  void **mapped_memories;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSet *descriptor_sets;
};

enum M_Result m_uniforms_create(M_Instance *instance);
void m_uniforms_destroy(M_Instance *instance);

#endif // !UNIFORM_MANAGEMENT_H
