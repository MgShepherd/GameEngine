#include "pipeline_management.h"
#include "instance_private.h"
#include "logger.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "vk_swap_chain_management.h"
#include "vk_utils.h"
#include <vulkan/vulkan_core.h>

struct M_PipelineStages {
  VkPipelineShaderStageCreateInfo vert_shader_stage;
  VkPipelineShaderStageCreateInfo frag_shader_stage;
  VkPipelineVertexInputStateCreateInfo vertex_input_state;
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
  VkPipelineViewportStateCreateInfo viewport_state;
  VkPipelineRasterizationStateCreateInfo rasterization_state;
  VkPipelineMultisampleStateCreateInfo multisample_state;
  VkPipelineColorBlendAttachmentState color_blend_attachment_state;
  VkPipelineColorBlendStateCreateInfo color_blend_state;
};

VkViewport create_viewport(const struct M_SwapChain *swap_chain) {
  return (VkViewport){
      .x = 0.0f,
      .y = 0.0f,
      .width = swap_chain->extent.width,
      .height = swap_chain->extent.height,
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };
}

VkRect2D create_scissor(const struct M_SwapChain *swap_chain) {
  return (VkRect2D){
      .offset = {.x = 0, .y = 0},
      .extent = swap_chain->extent,
  };
}

VkPipelineShaderStageCreateInfo create_shader_stage(const VkShaderModule module, VkShaderStageFlagBits stage) {
  return (VkPipelineShaderStageCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = stage,
      .module = module,
      .pName = "main",
  };
}

VkPipelineVertexInputStateCreateInfo create_vertex_input_state() {
  return (VkPipelineVertexInputStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 0,
      .vertexAttributeDescriptionCount = 0,
  };
}

VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state() {
  return (VkPipelineInputAssemblyStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };
}

VkPipelineViewportStateCreateInfo create_viewport_state(const VkViewport *viewport, const VkRect2D *scissor) {
  return (VkPipelineViewportStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .pScissors = scissor,
      .scissorCount = 1,
      .pViewports = viewport,
      .viewportCount = 1,
  };
}

VkPipelineRasterizationStateCreateInfo create_rasterization_state() {
  return (VkPipelineRasterizationStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .lineWidth = 1.0f,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
  };
}

VkPipelineMultisampleStateCreateInfo create_multisample_state() {
  return (VkPipelineMultisampleStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .sampleShadingEnable = VK_FALSE,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
  };
}

VkPipelineColorBlendAttachmentState create_color_blend_attachment_state() {
  return (VkPipelineColorBlendAttachmentState){
      .colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
      .blendEnable = VK_FALSE,
  };
}

VkPipelineColorBlendStateCreateInfo
create_color_blend_state(const VkPipelineColorBlendAttachmentState *color_blend_attachment) {
  return (VkPipelineColorBlendStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = color_blend_attachment,
  };
}

enum M_Result create_shader_stages(struct M_PipelineStages *stages, struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  VkShaderModule vert_shader_module, frag_shader_module;
  m_shader_modules_create(&vert_shader_module, &frag_shader_module, instance);
  return_result_if_err(result);

  const VkViewport viewport = create_viewport(&instance->swapchain);
  const VkRect2D scissor = create_scissor(&instance->swapchain);

  const VkPipelineColorBlendAttachmentState color_blend_attachment_state = create_color_blend_attachment_state();

  *stages = (struct M_PipelineStages){
      .vert_shader_stage = create_shader_stage(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT),
      .frag_shader_stage = create_shader_stage(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT),
      .vertex_input_state = create_vertex_input_state(),
      .input_assembly_state = create_input_assembly_state(),
      .viewport_state = create_viewport_state(&viewport, &scissor),
      .rasterization_state = create_rasterization_state(),
      .multisample_state = create_multisample_state(),
      .color_blend_attachment_state = color_blend_attachment_state,
      .color_blend_state = create_color_blend_state(&color_blend_attachment_state),
  };

  return result;
}

enum M_Result create_pipeline_layout(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  VkPipelineLayoutCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
  };

  VkResult vk_result =
      vkCreatePipelineLayout(instance->device.vk_device, &create_info, NULL, &instance->pipeline.layout);
  vk_return_result_if_err(vk_result);

  return result;
};

void destroy_shader_stages(struct M_PipelineStages *stages, const struct M_Instance *instance) {
  m_shader_modules_destroy(stages->vert_shader_stage.module, stages->frag_shader_stage.module, instance);
}

enum M_Result m_pipeline_create(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  struct M_PipelineStages stages;
  return_result_if_err(create_shader_stages(&stages, instance));
  return_result_if_err(create_pipeline_layout(instance));

  destroy_shader_stages(&stages, instance);

  return result;
}

void m_pipeline_destroy(M_Instance *instance) {
  if (instance->pipeline.layout != NULL) {
    vkDestroyPipelineLayout(instance->device.vk_device, instance->pipeline.layout, NULL);
    instance->pipeline.layout = NULL;
  }
}
