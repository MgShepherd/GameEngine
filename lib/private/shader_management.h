#ifndef SHADER_MANAGEMENT_H
#define SHADER_MANAGEMENT_H

#include "instance.h"
#include <vulkan/vulkan_core.h>

enum M_Result m_shader_modules_create(VkShaderModule *vert_shader, VkShaderModule *frag_shader, M_Instance *instance);
void m_shader_modules_destroy(VkShaderModule vert_shader, VkShaderModule frag_shader, const M_Instance *instance);

#endif // !SHADER_MANAGEMENT_H
