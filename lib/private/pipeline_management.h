#ifndef PIPELINE_MANAGEMENT_H
#define PIPELINE_MANAGEMENT_H

#include "instance.h"
#include <vulkan/vulkan_core.h>

struct M_Pipeline {
  VkPipelineLayout layout;
  VkDescriptorSetLayout descriptor_layout;
  VkRenderPass render_pass;
  VkSubpassDependency subpass_dependency;
  VkPipeline vk_pipeline;
};

enum M_Result m_pipeline_create(M_Instance *instance);
void m_pipeline_destroy(M_Instance *instance);

#endif // !PIPELINE_MANAGEMENT_H
