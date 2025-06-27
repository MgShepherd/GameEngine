#ifndef M_SHADER_MANAGEMENT_H
#define M_SHADER_MANAGEMENT_H

#include "cglm/types.h"
#include "instance.h"
#include <vulkan/vulkan_core.h>

struct M_Vertex {
  vec2 position;
  vec3 color;
};

enum M_Result m_shader_modules_create(VkShaderModule *vert_shader, VkShaderModule *frag_shader, M_Instance *instance);
void m_shader_modules_destroy(VkShaderModule vert_shader, VkShaderModule frag_shader, const M_Instance *instance);

void m_shader_get_input_binding(VkVertexInputBindingDescription *binding_description);
enum M_Result m_shader_get_input_attributes(VkVertexInputAttributeDescription **attribute_descriptions,
                                            uint32_t *num_attributes);

#endif
