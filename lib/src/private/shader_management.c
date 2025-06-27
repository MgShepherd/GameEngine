#include "shader_management.h"
#include "instance_private.h"
#include "logger.h"
#include "m_utils.h"
#include "result.h"
#include "result_utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

const uint32_t NUM_VERTEX_ATTRIBUTES = 2;

void cleanup_load_shader(FILE *file, uint32_t *file_contents) {
  if (file != NULL) {
    fclose(file);
  }
  if (file_contents != NULL) {
    free(file_contents);
  }
}

enum M_Result load_shader_contents(const char *file_name, uint32_t **file_contents, uint32_t *content_length) {
  enum M_Result result = M_SUCCESS;
  *file_contents = NULL;

  FILE *file = fopen(file_name, "rb");
  if (file == NULL) {
    return_result_if_err(m_result_process(M_FILE_READ_ERR, file_name));
  }

  int f_result = fseek(file, 0, SEEK_END);
  if (f_result != 0) {
    result = m_result_process(M_FILE_READ_ERR, "Unable to jump to end of file");
    return_result_if_err_clean(result, cleanup_load_shader, file, *file_contents);
  }

  long file_length = ftell(file);
  if (file_length <= 0) {
    result = m_result_process(M_FILE_READ_ERR, "Unable to process file contents");
    return_result_if_err_clean(result, cleanup_load_shader, file, *file_contents);
  }

  *file_contents = malloc(file_length);
  const char *memory_alloc_msg = "Unable to allocate memory for shader file contents";
  return_result_if_null_clean(*file_contents, M_MEMORY_ALLOC_ERR, memory_alloc_msg, cleanup_load_shader, file,
                              *file_contents);
  rewind(file);

  *content_length = fread(*file_contents, 1, file_length, file);
  if (*content_length != (uint32_t)file_length) {
    result = m_result_process(M_FILE_READ_ERR, "Incorrect number of bytes processed in file");
    return_result_if_err_clean(result, cleanup_load_shader, file, *file_contents);
  }

  fclose(file);
  return result;
}

void cleanup_shader_module_creation(uint32_t *vert_shader_contents, uint32_t *frag_shader_contents) {
  if (vert_shader_contents != NULL) {
    free(vert_shader_contents);
  }
  if (frag_shader_contents != NULL) {
    free(frag_shader_contents);
  }
}

enum M_Result create_shader_module(VkShaderModule *module, const struct M_Instance *instance, const uint32_t *contents,
                                   uint32_t length) {
  enum M_Result result = M_SUCCESS;

  const VkShaderModuleCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = length,
      .pCode = contents,
  };

  VkResult vk_result = vkCreateShaderModule(instance->device.vk_device, &create_info, NULL, module);
  vk_return_result_if_err(vk_result);

  return result;
}

enum M_Result m_shader_modules_create(VkShaderModule *vert_shader, VkShaderModule *frag_shader, M_Instance *instance) {
  enum M_Result result = M_SUCCESS;
  uint32_t *vert_shader_contents = NULL;
  uint32_t *frag_shader_contents = NULL;
  uint32_t vert_shader_length = 0, frag_shader_length = 0;

  result = load_shader_contents("lib/shaders/basic/vert.spv", &vert_shader_contents, &vert_shader_length);
  return_result_if_err_clean(result, cleanup_shader_module_creation, vert_shader_contents, frag_shader_contents);
  result = load_shader_contents("lib/shaders/basic/frag.spv", &frag_shader_contents, &frag_shader_length);
  return_result_if_err_clean(result, cleanup_shader_module_creation, vert_shader_contents, frag_shader_contents);

  result = create_shader_module(vert_shader, instance, vert_shader_contents, vert_shader_length);
  return_result_if_err_clean(result, m_shader_modules_destroy, *vert_shader, *frag_shader, instance);
  result = create_shader_module(frag_shader, instance, frag_shader_contents, frag_shader_length);
  return_result_if_err_clean(result, m_shader_modules_destroy, *vert_shader, *frag_shader, instance);

  cleanup_shader_module_creation(vert_shader_contents, frag_shader_contents);
  return result;
}

void m_shader_modules_destroy(VkShaderModule vert_shader, VkShaderModule frag_shader,
                              const struct M_Instance *instance) {
  if (vert_shader != NULL) {
    vkDestroyShaderModule(instance->device.vk_device, vert_shader, NULL);
  }
  if (frag_shader != NULL) {
    vkDestroyShaderModule(instance->device.vk_device, frag_shader, NULL);
  }
}

void m_shader_get_input_binding(VkVertexInputBindingDescription *binding_description) {
  *binding_description = (VkVertexInputBindingDescription){
      .binding = 0,
      .stride = sizeof(struct M_Vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };
}

enum M_Result m_shader_get_input_attributes(VkVertexInputAttributeDescription **attribute_descriptions,
                                            uint32_t *num_attributes) {
  enum M_Result result = M_SUCCESS;
  *attribute_descriptions = malloc(NUM_VERTEX_ATTRIBUTES * sizeof(VkVertexInputAttributeDescription));
  return_result_if_null(*attribute_descriptions, M_MEMORY_ALLOC_ERR, "Unable to allocate memory");

  *num_attributes = NUM_VERTEX_ATTRIBUTES;
  (*attribute_descriptions)[0] = (VkVertexInputAttributeDescription){
      .binding = 0,
      .location = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(struct M_Vertex, position),
  };

  (*attribute_descriptions)[1] = (VkVertexInputAttributeDescription){
      .binding = 0,
      .location = 1,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = offsetof(struct M_Vertex, color),
  };

  return result;
}
