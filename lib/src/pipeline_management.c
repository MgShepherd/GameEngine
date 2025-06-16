#include "pipeline_management.h"
#include "logger.h"
#include "result.h"
#include "result_utils.h"
#include "shader_management.h"
#include <vulkan/vulkan_core.h>

struct M_PipelineStages {
  VkPipelineShaderStageCreateInfo vert_shader_stage;
  VkPipelineShaderStageCreateInfo frag_shader_stage;
};

VkPipelineShaderStageCreateInfo create_shader_stage(const VkShaderModule module, VkShaderStageFlagBits stage) {
  return (VkPipelineShaderStageCreateInfo){
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = stage,
      .module = module,
      .pName = "main",
  };
}

enum M_Result create_shader_stages(struct M_PipelineStages *stages, struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  VkShaderModule vert_shader_module, frag_shader_module;
  m_shader_modules_create(&vert_shader_module, &frag_shader_module, instance);
  return_result_if_err(result);

  *stages = (struct M_PipelineStages){
      .vert_shader_stage = create_shader_stage(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT),
      .frag_shader_stage = create_shader_stage(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT),
  };

  return result;
}

void destroy_shader_stages(struct M_PipelineStages *stages, const struct M_Instance *instance) {
  m_shader_modules_destroy(stages->vert_shader_stage.module, stages->frag_shader_stage.module, instance);
}

enum M_Result m_pipeline_create(struct M_Instance *instance) {
  enum M_Result result = M_SUCCESS;

  struct M_PipelineStages stages;
  return_result_if_err(create_shader_stages(&stages, instance));

  destroy_shader_stages(&stages, instance);

  return result;
}

void m_pipeline_destroy(M_Instance *instance) {}
