#include "pipeline_management.h"
#include "instance_private.h"
#include "logger.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include "vk_swap_chain_management.h"
#include "vk_utils.h"
#include <assert.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_SHADER_STAGES = 2;

struct M_PipelineStages {
  VkViewport *viewport;
  VkRect2D *scissor;
  VkVertexInputBindingDescription shader_binding_description;
  VkVertexInputAttributeDescription *shader_attribute_descriptions;
  uint32_t num_shader_attribute_descriptions;
  VkPipelineShaderStageCreateInfo *shader_stages;
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

VkPipelineVertexInputStateCreateInfo create_vertex_input_state(struct M_PipelineStages *stages) {
  assert(stages->shader_attribute_descriptions != NULL);

  return (VkPipelineVertexInputStateCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &stages->shader_binding_description,
      .vertexAttributeDescriptionCount = stages->num_shader_attribute_descriptions,
      .pVertexAttributeDescriptions = stages->shader_attribute_descriptions,
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

void cleanup_shader_stage_create(VkShaderModule vert_shader, VkShaderModule frag_shader, VkViewport *viewport,
                                 VkRect2D *scissor, const struct M_Instance *instance) {
  m_shader_modules_destroy(vert_shader, frag_shader, instance);
  if (viewport != NULL) {
    free(viewport);
  }
  if (scissor != NULL) {
    free(scissor);
  }
}

void destroy_shader_stages(struct M_PipelineStages *stages, const M_Instance *instance) {
  if (stages->shader_stages != NULL) {
    cleanup_shader_stage_create(stages->shader_stages[0].module, stages->shader_stages[1].module, stages->viewport,
                                stages->scissor, instance);

    if (stages->shader_attribute_descriptions != NULL) {
      free(stages->shader_attribute_descriptions);
    };
    free(stages->shader_stages);
    stages->shader_stages = NULL;
    stages->shader_attribute_descriptions = NULL;
  }
}

enum M_Result create_shader_stages(struct M_PipelineStages *stages, struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  VkViewport *viewport = NULL;
  VkRect2D *scissor = NULL;

  VkShaderModule vert_shader_module, frag_shader_module;
  m_shader_modules_create(&vert_shader_module, &frag_shader_module, instance);
  return_result_if_err(result);

  viewport = malloc(sizeof(VkViewport));
  return_result_if_null_clean(viewport, M_MEMORY_ALLOC_ERR, "Unable to allocate memory", cleanup_shader_stage_create,
                              vert_shader_module, frag_shader_module, viewport, scissor, instance);
  scissor = malloc(sizeof(VkRect2D));
  return_result_if_null_clean(viewport, M_MEMORY_ALLOC_ERR, "Unable to allocate memory", cleanup_shader_stage_create,
                              vert_shader_module, frag_shader_module, viewport, scissor, instance);
  *viewport = create_viewport(&instance->swapchain);
  *scissor = create_scissor(&instance->swapchain);

  const VkPipelineColorBlendAttachmentState color_blend_attachment_state = create_color_blend_attachment_state();

  VkPipelineShaderStageCreateInfo *shader_stages_create_info =
      malloc(NUM_SHADER_STAGES * sizeof(VkPipelineShaderStageCreateInfo));
  return_result_if_null_clean(shader_stages_create_info, M_MEMORY_ALLOC_ERR, "Unable to allocate memory",
                              cleanup_shader_stage_create, vert_shader_module, frag_shader_module, viewport, scissor,
                              instance);
  shader_stages_create_info[0] = create_shader_stage(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
  shader_stages_create_info[1] = create_shader_stage(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

  *stages = (struct M_PipelineStages){
      .viewport = viewport,
      .scissor = scissor,
      .shader_stages = shader_stages_create_info,
      .input_assembly_state = create_input_assembly_state(),
      .viewport_state = create_viewport_state(viewport, scissor),
      .rasterization_state = create_rasterization_state(),
      .multisample_state = create_multisample_state(),
      .color_blend_attachment_state = color_blend_attachment_state,
      .color_blend_state = create_color_blend_state(&color_blend_attachment_state),
  };

  m_shader_get_input_binding(&stages->shader_binding_description);
  result =
      m_shader_get_input_attributes(&stages->shader_attribute_descriptions, &stages->num_shader_attribute_descriptions);
  return_result_if_err_clean(result, destroy_shader_stages, stages, instance);
  stages->vertex_input_state = create_vertex_input_state(stages);

  return result;
}

VkAttachmentDescription create_color_attachment(const struct M_Instance *instance) {
  return (VkAttachmentDescription){
      .format = instance->swapchain.format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };
}

VkAttachmentReference create_color_ref() {
  return (VkAttachmentReference){
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .attachment = 0,
  };
}

VkSubpassDescription create_subpass(const VkAttachmentReference *color_ref) {
  return (VkSubpassDescription){
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = color_ref,
  };
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

void create_subpass_dependency(struct M_Instance *instance) {
  instance->pipeline.subpass_dependency = (VkSubpassDependency){
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
  };
}

enum M_Result create_render_pass(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  const VkAttachmentDescription color_attachment = create_color_attachment(instance);
  const VkAttachmentReference color_attachment_ref = create_color_ref();
  const VkSubpassDescription subpass = create_subpass(&color_attachment_ref);

  const VkRenderPassCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &color_attachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &instance->pipeline.subpass_dependency,
  };

  VkResult vk_result =
      vkCreateRenderPass(instance->device.vk_device, &create_info, NULL, &instance->pipeline.render_pass);
  vk_return_result_if_err_clean(vk_result, m_pipeline_destroy, instance);

  return result;
}

enum M_Result m_pipeline_create(struct M_Instance *instance) {
  assert(instance->vk_instance != NULL && instance->device.vk_device != NULL);
  enum M_Result result = M_SUCCESS;

  struct M_PipelineStages stages;
  return_result_if_err(create_shader_stages(&stages, instance));
  return_result_if_err(create_pipeline_layout(instance));
  create_subpass_dependency(instance);
  return_result_if_err(create_render_pass(instance));

  const VkGraphicsPipelineCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = 2,
      .pStages = stages.shader_stages,
      .pVertexInputState = &stages.vertex_input_state,
      .pInputAssemblyState = &stages.input_assembly_state,
      .pViewportState = &stages.viewport_state,
      .pRasterizationState = &stages.rasterization_state,
      .pMultisampleState = &stages.multisample_state,
      .pColorBlendState = &stages.color_blend_state,
      .layout = instance->pipeline.layout,
      .renderPass = instance->pipeline.render_pass,
      .subpass = 0,
  };

  VkResult vk_result = vkCreateGraphicsPipelines(instance->device.vk_device, NULL, 1, &create_info, NULL,
                                                 &instance->pipeline.vk_pipeline);

  destroy_shader_stages(&stages, instance);
  vk_return_result_if_err_clean(vk_result, m_pipeline_destroy, instance);

  return result;
}

void m_pipeline_destroy(M_Instance *instance) {
  if (instance->pipeline.vk_pipeline != NULL) {
    vkDestroyPipeline(instance->device.vk_device, instance->pipeline.vk_pipeline, NULL);
    instance->pipeline.vk_pipeline = NULL;
  }
  if (instance->pipeline.render_pass != NULL) {
    vkDestroyRenderPass(instance->device.vk_device, instance->pipeline.render_pass, NULL);
    instance->pipeline.render_pass = NULL;
  }
  if (instance->pipeline.layout != NULL) {
    vkDestroyPipelineLayout(instance->device.vk_device, instance->pipeline.layout, NULL);
    instance->pipeline.layout = NULL;
  }
}
